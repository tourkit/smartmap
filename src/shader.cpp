#include "engine.hpp"
#include "shader.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "model.hpp"

#include <GL/gl3w.h>
#include <chrono>

#include <set>
#include "struct.hpp"

//// SHADERBUILDER


static std::string camel(std::string str) { str[0] = std::toupper(str[0]); return str; }
static std::string lower(std::string str) { str[0] = std::tolower(str[0]); return str; }

std::string ShaderProgram::Builder::struct_(Member* s) {

    std::string str = "struct " + camel(s->name())  + " {";

    for (auto m : s->members) {

        while (m->members.size() == 1) m = m->members[0];

        if (!m->typed()) {

            if (m->quantity()) str +=struct_(m);

        }else{

            str += " ";
            str += m->type_name();
            str += " " + m->name() + ";";

        }
    
    }

    if (s->stride()) for (int i = 0; i < s->stride()/sizeof(float); i++) str += " float stride" + std::to_string(i) + ";";

    str += "}";

    return str;

}

std::string ShaderProgram::Builder::layout(UBO* ubo) {

    if (!ubo->members.size() || !ubo->data.size()) return "";

    std::string str = "layout (binding = " + std::to_string(ubo->binding) + ", std140) uniform " + ubo->name() + " {";

    for (auto m : ubo->members) {

        while (m->members.size() == 1) m = m->members[0];

        if (!m->typed()) {

            str += " " + struct_(m) + " " + lower(m->name()) ;
    
            if (m->quantity()>1) str += "["+std::to_string(m->quantity())+"]";

            str += " ;";

        }else{

            str += " ";
            str += m->type_name();
            str += " " + m->name() + ";";

        }
    
    }

    if (ubo->stride()) for (int i = 0; i < ubo->stride()/sizeof(float); i++) str += " float stride" + std::to_string(stride_count++) + ";";
    
    str += " };\n\n";
    
    return str;

}


ShaderProgram::Builder::Builder() {

    header_common = "#version 430 core\n\n"+comment_line;

    stride_count = 0;

    header_common += layout(&engine.dynamic_ubo);
    header_common += layout(&engine.static_ubo);

}

std::string ShaderProgram::Builder::frag(std::vector<Model> &models) {

    std::string str = header_common;

    // str += "uniform sampler2D texture0;\n\n"; // foreach declared Texture::units maybe ? 
    // str += "uniform sampler2D medias;\n\n";

    // str += "vec4 color;\n\n";
    // str += "vec2 uv;\n\n";

    str += "in vec2 UV;\n\n";
    str += "out vec4 COLOR;\n\n";

    // str += comment_line;

    // std::set<Effector*> effectors;
    
    // for (auto &m : vbo.models) for (auto effector : m->effectors) effectors.insert(effector);
    // for (auto effector : effectors) str += effector->source() +"\n";
    
    str += "\n";

    // main loop
    str += "void main() {\n\n";
    str += "\tCOLOR = vec4(1);\n\n";

    // tofix

    int model_id = 0;

    for (auto &model : models) {

        for (int instance = 0; instance < model.quantity; instance++) {

    //         auto name = model->file->name+std::to_string(model_id)+"["+std::to_string(instance)+"]";

    //         str += "\t// "+name+"\n"; // would love this to be a node name instead
    //         str += "\tuv = UV;\n";
    //         str += "\tcolor = vec4(1);\n";
            
    //         for (auto effector : model->effectors) { 
                
    //             std::string arg_str;

    //             auto comp = Component::id(effector->file->name.c_str()); 

    //             if (comp->members.size()<2) arg_str += name+"."+effector->file->name;

    //             else for (auto &m: comp->members) {
                    
    //                 arg_str += name+"."+effector->file->name+"."+m.name;

    //                 if (&m != &comp->members.back()) arg_str += ", ";
                    
    //             }

    //             str += "\t"+effector->file->name+"("+arg_str+");\n";
    //         }

    //         str += "\tCOLOR += color;\n\n";

        }


    //     str += "\n\n";

        model_id++;
    }
    
    str += "} ";

    return str;

}

std::string ShaderProgram::Builder::vert(std::vector<Model> &models) {

    std::string str = header_common;

    str += "layout (location = 0) in vec2 POSITION;\n";
    str += "layout (location = 1) in vec2 TEXCOORD;\n";
    // str += "layout (location = 3) in int OBJ;\n\n";

    str += "out vec2 UV;\n\n";

    str += "\nvoid main() {\n\n";

    str += "\tUV = TEXCOORD;\n";
    // str += "\tUV.y = 1-UV.y;\n\n";

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
        PLOGW << (type==1?"vertex: ":"fragment: ") << &infoLog[7];
        PLOGV <<source;
        
    }

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { destroy(); }

ShaderProgram::ShaderProgram() { std::vector<Model> none; Builder builder; create(builder.frag(none),builder.vert(none)); }

ShaderProgram::ShaderProgram(std::vector<Model> &models) { Builder builder; create(builder.frag(models),builder.vert(models)); }

ShaderProgram::ShaderProgram(std::string frag, std::string vert) { create(frag,vert); }

void ShaderProgram::destroy() {  

    loaded = false;
    if (id > -1) glDeleteProgram(id); 

}

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


