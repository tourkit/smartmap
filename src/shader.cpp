#include "shader.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "vbo.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "atlas.hpp"
#include "utils.hpp"

#include <GL/gl3w.h>
#include <chrono>

#include <set>
#include "struct.hpp"
#include "texture.hpp"

//// SHADERBUILDER


void ShaderProgram::Builder::build() {

    header_common = version;

    header_common += layout();

    header_fragment = "out vec4 COLOR;\n\n";

    for (auto x : samplers) header_fragment += "uniform sampler2D "+x+";\n"; if (samplers.size()) header_fragment += "\n";

    header_vertex.clear();

    if (vbo) {

        int count = 0;

        for (auto x : vbo->vertice->members) {

            header_vertex += "layout (location = "+std::to_string(count)+") in "+x->type_name()+" "+x->name()+(count?"_":"")+";\n";

            count ++;

        }

        header_vertex += "\n";

    }

    effectors_fragment_str.clear();
    for (auto x : effectors_fragment)  effectors_fragment_str += x->source()+"\n\n";

    effectors_vertex_str.clear();

    for (auto x : effectors_vertex)  {

        effectors_vertex_str += x->source()+"\n\n";

        header_vertex += x->source()+"\n\n";

    }

    body_fragment.clear();
    body_vertex.clear();
}

std::string ShaderProgram::Builder::frag() {

    return header_common + comment_line + header_fragment + (header_fragment.length()?comment_line:"") + effectors_fragment_str + (effectors_fragment_str.length()?comment_line:"") + "void main() {\n\n" + (body_fragment.length()?body_fragment:"\tCOLOR = vec4(0,1,0,1);\n\n") + "}\n\n" + comment_line;

}

std::string ShaderProgram::Builder::vert() {

    return header_common + comment_line + (header_vertex.length()?header_vertex:"layout (location = 0) in vec2 POSITION;\n") + (header_vertex.length()?comment_line:"") + effectors_vertex_str + (effectors_vertex_str.length()?comment_line:"") + comment_line + "void main() {\n\n" + (body_vertex.length()?body_vertex:"\tvec2 POS = POSITION;\n\n\tgl_Position = vec4(POS, 0, 1);\n\n") + "}\n\n" + comment_line;

}


bool ShaderProgram::Builder::add(UBO* ubo) { return true; }

std::string ShaderProgram::Builder::layout() {

    std::vector<Member*> structs;

    for (auto ubo : ubos)    {

        ubo->each([&](Instance& inst) {

            auto m = inst.def();

            if (m->type() == typeid(Struct))
                ADD_UNIQUE<Member*>(structs,m->ref()?m->ref():m);

        });

        ADD_UNIQUE<Member*>(structs,ubo);

    }

    std::string out;

    std::map<Member*,std::string> unique_name_list;

    for (auto m : structs) {

        auto name = camel(m->name());

        while (true) {

            for (auto &x : unique_name_list) if (x.second == name) {

                name += "_";

                continue;

            }

            break;

        }

        unique_name_list[m] = name;

    }

    for (auto x : structs) { auto def = print_struct(x,unique_name_list); if (def.length()) def+=";\n\n"; out += def; }

    for (auto ubo : ubos) {

        out += "layout (binding = " + std::to_string(ubo->binding) + ", std140) uniform " + ubo->name() + "_ ";

        out += " { " + camel(ubo->name()) + " " + lower(ubo->name());

        if (ubo->quantity()>1) out += "["+std::to_string(ubo->quantity())+"]";

        out += + "; };\n\n";

    }

    return out;

}

std::string ShaderProgram::Builder::print_struct(Member* member, std::map<Member*,std::string> &unique_name_list) {

    if (!member->size()) return "";

    std::string out;
    std::string nl = "";
    std::string tb = "";
    // if (member->members.size() == 1) nl = "";

    std::string content;

    for (auto x : member->ref()?member->ref()->members:member->members) {

        if (!x->size()) continue;

        auto x_ = x->ref()?x->ref():x;

        auto name = unique_name_list.find(x_)!=unique_name_list.end()?unique_name_list[x_]:x_->type_name();

        content+=tb+""+name+" "+lower(x->ref()?x->type_name():x->name());

        if (x->quantity()>1) content += "["+std::to_string(x->quantity())+"]";

        content += "; "+nl;

    }

    if (!content.length()) return "";

    out+="struct "+unique_name_list[member]+" { "+nl+nl+content;

    if (member->stride()) for (int i = 0; i < member->stride()/sizeof(float); i++) {

        out += tb;
        out += (member->members.back()->type() == typeid(int) ? "int" : "float");
        out += " stride";
        out += std::to_string(i) + "; "+nl ;

    }

    out+=nl+"}";
    return out;

}



//// SHADER

Shader::Shader() { }

Shader::~Shader() { if (id > -1) glDeleteShader(id);  }

Shader::Shader(std::string src, uint8_t type)  { create(src,type); }

bool Shader::create(std::string src, uint8_t type)  {

    this->src = src;

    auto gl_type = GL_FRAGMENT_SHADER;

    if (type == 1) gl_type = GL_VERTEX_SHADER;
    if (type == 2) gl_type = GL_COMPUTE_SHADER;

    id = glCreateShader(gl_type);

    const GLchar *source = src.c_str();
    glShaderSource(id, 1, &source, nullptr);

    glCompileShader(id);

    // Check for errors

    memset(&infoLog[0],0,512);

    GLint success;

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(id, 512, NULL, infoLog);
        auto nl = std::strchr(infoLog, '\n');
        if (nl) std::memset(nl, 0, 1);
        PLOGE << (type==1?"vertex: ":"fragment: ") << &infoLog[7];
        PLOGV <<source;

    }

    return success;

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { destroy();

    if (owned)
        delete builder_v;

}

ShaderProgram::ShaderProgram() { Builder builder; builder.build(); create(builder.frag(),builder.vert()); }

ShaderProgram::ShaderProgram(std::string frag, std::string vert) { create(frag,vert); }

void ShaderProgram::destroy() {

    loaded = false;
    if (id > -1)
        glDeleteProgram(id);

}


ShaderProgram::Builder* ShaderProgram::builder() {

    if (!builder_v) {

        builder_v = new Builder();

        owned = true;

    }

    return builder_v;

}

bool ShaderProgram::builder(Builder* builder) {

    if (owned)
        delete builder_v;

    builder_v = builder; owned = false;

    return true;

}

void  ShaderProgram::create() {

    if (builder_v)
        create(builder_v);

}

void  ShaderProgram::create(Builder* builder) { builder->build(); create(builder->frag(), builder->vert()); }

void  ShaderProgram::create(std::string frag_src, std::string vert_src) {

    destroy();

    id = glCreateProgram();

    frag.create(frag_src,0);
    glAttachShader(id, frag.id);

    vert.create(vert_src,1);
    glAttachShader(id, vert.id);

    glLinkProgram( id );

    loaded = true;

    last_change = std::chrono::system_clock::now();

    use();

}

void ShaderProgram::use() {  glUseProgram(id); }

void ShaderProgram::use(uint32_t x, uint32_t y, uint32_t z) {  glUseProgram(id); glDispatchCompute(x,y,z); }

ShaderProgram::operator uint32_t() { return id; }

int ShaderProgram::getLoc(const std::string& name) { return glGetUniformLocation(id, name.c_str()); }

void ShaderProgram::sendUniform(const std::string& name, int i1) { glProgramUniform1i(id, getLoc(name), i1); }

void ShaderProgram::sendUniform(const std::string& name, float f1) { glProgramUniform1f(id, getLoc(name), f1); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2) { glProgramUniform2f(id, getLoc(name), f1, f2); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2, float f3) { glProgramUniform3f(id, getLoc(name), f1, f2, f3); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2, float f3, float f4) { glProgramUniform4f(id, getLoc(name), f1, f2, f3, f4); }

//void Shader::sendUniform(std::string name, glm::mat4 mat) { glProgramUniformMatrix4fv(program, getLoc(name), 1, GL_FALSE, &mat[0][0]); }
