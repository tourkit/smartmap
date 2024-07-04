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

//// SHADERBUILDER


static std::string print_recurse(Member* _this, int recurse=-1, int depth=0) {

    static const char* new_line = "\n";
    static const char* tab = "\t";

    static std::set<std::string> used;

    if (!depth) used.clear();

    if (false) { new_line = ""; tab = ""; }

    std::string tab_str;
    for (int i = 0 ; i < depth; i++) tab_str+=tab;
    std::string str;

    for (auto m : _this->members) {

        if (m != _this->members[0]) str += tab_str+tab;

        if ( m->isData() || m->ref() ) {

            if (m->ref() && m->members.size()) str += camel(m->type_name());

            else str += m->type_name();

        } else {

            if (recurse) {

                auto m_str = print_recurse(m, recurse-1, depth+1);

                if (!m_str.length()) continue;

                str += m_str;

            } else { str += camel(m->name()); }

        }

        str += " " + lower(m->name());

        if (m->quantity() > 1) str += "[" + std::to_string(m->quantity()) + "]";

        str += "; ";
        str += new_line;


    }

    if (_this->stride()) for (int i = 0; i < _this->stride()/sizeof(float); i++) {

        str += tab_str+tab;
        if (!new_line) str += " ";
        str += (_this->members[0]->type() == typeid(int) ? "int" : "float");
        str += " stride";
        str += std::to_string(i) + "; " + new_line;

    }

    if (!str.length()) return "";

    std::string struct_name = camel(_this->name());

    while (used.find(struct_name) != used.end()) struct_name+="_"; // GLSL DOES NOT DIFFERENCIATE NESTED STRUCTS, HERE Bar() WOULD CONFLICT : struct Bar{}; struct Foo { struct Bar {}; };

    used.insert(struct_name);

    return "struct " + struct_name  + " { " + new_line+new_line + tab_str +tab+ str + new_line + tab_str + "}";

}


void ShaderProgram::Builder::build() {

    common();

    frag();

    vert();

    vertex.clear();
    vertex += header_common;
    vertex += header_vertex;
    vertex += "\nvoid main() {\n\n";
    vertex += (body_vertex.length() ? body_vertex : "\tvec2 POS = POSITION;\n\n");;
    vertex += "\tgl_Position = vec4(POS, 0, 1);\n\n";
    vertex += "}\n////////////";

    fragment.clear();
    fragment += header_common;
    fragment += comment_line;
    fragment += "out vec4 COLOR;\n\n";
    fragment += header_fragment;
    fragment += "void main() {\n\n";
    fragment += (body_fragment.length() ? body_fragment : "\tCOLOR = vec4(0,1,0,1);\n\n");
    fragment += "}\n////////////";

}

void ShaderProgram::Builder::frag() { body_fragment.clear(); }

void ShaderProgram::Builder::common() {

    header_common = version;

    // for (auto x : ubos) header_common = define(x);
    // for (auto x : structs) header_common = define(x);

}

void ShaderProgram::Builder::vert() {

    body_vertex.clear();
    header_vertex.clear();

    if (!vbo) return;
    int count = 0;
    for (auto x : vbo->vertice->members) header_vertex += "layout (location = "+std::to_string(count++)+") in "+x->type_name()+" "+x->name()+(count?"_":"")+";\n";
    header_vertex += "\n";


}

std::string ShaderProgram::Builder::print_structs() {

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

    for (auto x : structs) { auto def = define(x,unique_name_list); if (def.length()) def+=";\n\n"; out += def; }

    return out;

}

std::string ShaderProgram::Builder::define(Member* member, std::map<Member*,std::string> &unique_name_list) {

    if (!member->size()) return "";

    std::string out;
    std::string nl = "";
    std::string tb = "";
    // if (member->members.size() == 1) nl = "";

    std::string content;

    for (auto x : member->ref()?member->ref()->members:member->members) {

        if (!x->size()) continue;

        auto name = unique_name_list.find(x)!=unique_name_list.end()?unique_name_list[x]:x->type_name();

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

std::string ShaderProgram::Builder::ubo_layout(std::vector<UBO*> ubos) {

    if (!ubos.size() || !ubos[0]->members.size() || !ubos[0]->data.size()) return "";

    std::string output;

    list.clear();

    std::vector<Member*> order;

    for (auto ubo : ubos) {

        list[ubo] = camel(ubo->name());

        ubo->each([&](Instance& inst) {

            auto m = inst.def();

            std::string ttt = m->type().name();
            auto *ubo_ = ubo;
            if (m->type() == typeid(Struct)) {

                if (m->ref()) m = m->ref();

                for (auto &x : list) if (x.first == m) return;

                auto name = camel(m->name());

                for (auto &x : list) if (x.second == name) name += "_";

                list[m] = name;

                order.push_back(m);
            }

        });

        order.push_back(ubo);

    }

    // for (auto x : order) { auto def = define(x, list[x]); if (def.length()) def+=";\n\n"; output += def; }

    for (auto ubo : ubos) {

        output += "layout (binding = " + std::to_string(ubo->binding) + ", std140) uniform " + ubo->name() + "_ ";

        output += " { " + camel(ubo->name()) + " " + lower(ubo->name());

        if (ubo->quantity()>1) output += "["+std::to_string(ubo->quantity())+"]";

        output += + "; };\n\n";

    }

    return output;

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

ShaderProgram::ShaderProgram() { Builder builder; create(builder.fragment,builder.vertex); }

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

void  ShaderProgram::create(Builder* builder) { builder->build(); create(builder->fragment, builder->vertex); }

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
