#include "engine.hpp"
#include "shader.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "drawcall.hpp"
#include "model.hpp"
#include "atlas.hpp"

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

        if ( m->isData() || m->isRef() ) {

            if (m->isRef() && m->members.size()) str += camel(m->type_name());

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
std::string ShaderProgram::Builder::layout(UBO* ubo) {

    if (!ubo->members.size() || !ubo->data.size()) return "";

    std::string str;

    str += "layout (binding = " + std::to_string(ubo->binding) + ", std140) uniform " + ubo->name() + " ";

    auto s = print_recurse(ubo);
    // auto s = ubo->print_recurse();

    str += s.c_str()+s.find("{");

    str += ";\n\n";

    return str;

}

ShaderProgram::Builder::Builder() : Builder(nullptr) { }

ShaderProgram::Builder::Builder(DrawCall* dc) : dc(dc) { build(); }

void ShaderProgram::Builder::build() {

    stride_count = 0;

    effectors.clear();

    for (auto dc_ : engine.stack->childrens) { // filling bad here

        auto dc = dc_->is_a<Layer>();

        for (auto &model : dc->models) for (auto &effector : model.get()->effectors) effectors.insert(effector->file);

        for (auto &effector : dc->effectors) effectors.insert(effector->file);

    }

    for (auto file : effectors)  layouts += Effector::get(file).s.print_recurse()+";\n\n";

    layouts += layout(&engine.static_ubo);
    layouts += layout(&engine.dynamic_ubo);

}


std::string ShaderProgram::Builder::frag() {

    std::string str = header_common;

    str += comment_line;

    // str += "//uniform sampler2D texture0;\n\n"; // foreach declared Texture::units maybe ?
    str += "uniform sampler2D medias;\n\n";
    str += "uniform sampler2D render_pass;\n\n";

    str += "in vec2 UV;\n\n";
    str += "out vec4 COLOR;\n\n";
    str += "in flat int ID;\n\n";

    str += "vec2 uv = UV;\n\n";
    str += "vec4 color = vec4(0);\n\n";

    if (effectors.size()) str += comment_line;

    str += layouts;

    int model_id = 0;

    for (auto file : effectors)  str += Effector::source(file)+";\n\n";

    str += "void tic() { COLOR += color; uv = UV; color = vec4(1); }\n\n";
    str += "void tac() { COLOR += color; uv = UV; color = vec4(0); }\n\n";

    str += comment_line;

    // main loop
    str += "void main() {\n\n";

    if (dc) for (auto &model : dc->models) {

        for (int instance = 0; instance < model.get()->s.quantity(); instance++) {

            auto name = model.get()->s.name();

            if (model.get()->s.quantity() > 1) name += "["+std::to_string(instance)+"]";

            str += "\t// "+name+"\n"; // would love this to be a node name instead // still matters ?
            str += "\ttic();\n"; // would love this to be a node name instead // still matters ?

            for (auto &effector : model.get()->effectors) {

                std::string arg_str;

                for (auto &arg : Effector::get(effector.get()->file).args) {

                    arg_str += dc->s.name()+"."+lower(name)+"."+effector->ref.name()+"."+arg.second+", ";

                }

                arg_str.resize(arg_str.size()-2);

                str += "\t"+effector->file->name()+"("+arg_str+");\n";
            }

        }

        str += "\ttac();\n\n";

        str += "\n";

        model_id++;
    }

    if (dc) {

        for (auto &effector : dc->effectors) {

            std::string arg_str;

            for (auto &arg : Effector::get(effector.get()->file).args) {

                arg_str += dc->s.name()+"."+effector->ref.name()+"."+arg.second+", ";

            }

            arg_str.resize(arg_str.size()-2);

            str += "\t"+effector->file->name()+"("+arg_str+");\n";
        }

        if (dc->effectors.size()) str += "\ttac();\n\n";

    }


    str += "COLOR=vec4(1,1,1,1);} ";

    return str;

}

std::string ShaderProgram::Builder::vert() {

    std::string str = header_common;

    str += "layout (location = 0) in vec2 POSITION;\n";
    str += "layout (location = 1) in vec2 TEXCOORD;\n";
    str += "layout (location = 3) in int OBJ;\n\n";

    str += layouts;

    str += "out vec2 UV;\n\n";
    str += "out int ID;\n\n";

    str += "\nvoid main() {\n\n";

    str += "\tUV = TEXCOORD;\n";
    str += "\tUV.y = 1-UV.y;\n\n";
    str += "\tID = gl_InstanceID;\n\n";
    str += "\t// vec2 pos = POSITION*layer[ID].size+layer[ID].pos;\n\n";

    str += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

    str += "}";

    return str;

}

//// SHADER

Shader::Shader() { }

Shader::~Shader() { if (id > -1) glDeleteShader(id);  }

Shader::Shader(std::string src, uint8_t type)  { create(src,type); }

void Shader::create(std::string src, uint8_t type)  {

    this->src = src;

    auto gl_type = GL_FRAGMENT_SHADER;

    if (type == 1) gl_type = GL_VERTEX_SHADER;
    if (type == 2) gl_type = GL_COMPUTE_SHADER;

    id = glCreateShader(gl_type);

    const GLchar *source = src.c_str();
    glShaderSource(id, 1, &source, nullptr);

    glCompileShader(id);

    // Check for errors

    GLchar infoLog[512];
    GLint success;

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::memset(std::strchr(infoLog, '\n'), 0, 1);
        PLOGE << (type==1?"vertex: ":"fragment: ") << &infoLog[7];
        PLOGV <<source;

    }

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { destroy(); }

ShaderProgram::ShaderProgram() { Builder builder; create(builder.frag(),builder.vert()); }

ShaderProgram::ShaderProgram(DrawCall* dc) { Builder builder(dc); create(builder.frag(),builder.vert()); }

ShaderProgram::ShaderProgram(std::string frag, std::string vert) { create(frag,vert); }

void ShaderProgram::destroy() {

    loaded = false;
    if (id > -1) glDeleteProgram(id);

}

void  ShaderProgram::create(DrawCall* dc) {

    Builder builder(dc);

    create(builder.frag(),builder.vert());

    engine.dynamic_ubo.bind(this);

    engine.static_ubo.bind(this);

}

void  ShaderProgram::create(std::string frag_src, std::string vert_src) {

    destroy();

    id = glCreateProgram();

    frag.create(frag_src,0);
    glAttachShader(id, frag.id);

    vert.create(vert_src,1);
    glAttachShader(id, vert.id);

    glLinkProgram( id );

    // engine.atlas->link(this);

    sendUniform("medias", 1);
    sendUniform("render_pass", 2);

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
