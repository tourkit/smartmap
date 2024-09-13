#include "shader.hpp"
#include "log.hpp"
#include "instance.hpp"
#include "model.hpp"
#include "atlas.hpp"
#include "builder.hpp"


#include "utils.hpp"

#include <GL/gl3w.h>
#include <algorithm>
#include <chrono>
#include <regex>

#include <set>
#include "struct.hpp"
#include "texture.hpp"



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

        glGetShaderInfoLog(id, 512, NULL, &infoLog[0]);

        std::string line = infoLog;
        // std::string output;;

        // std::istringstream iss(infoLog);

        // for (std::string line; std::getline(iss, line); ) {
            
        //     // std::smatch match;
        //     // int coords[3] = {0,0,0};
        //     // std::string val;
        //     // std::regex color_regex(".+([0-9]+):([0-9]+):([0-9]+|(\\([0-9]+\\))):( error:)? (.+)");
        //     // if (std::regex_search(line, match, color_regex)) {

        //     //     coords[0] = std::stoi(match[1].str());
        //     //     coords[1] = std::stoi(match[2].str());
        //     //     if (match.size()>2)
        //     //         coords[2] = std::stoi(match[3].str());
        //     //     if (match.size()>5)
        //     //         val = match[6].str();
        //     // }

        //     // output += ">> " + line + "\n";
            


        // }

        // PLOGW << output;;


        auto first_line_break = line.find('\n');

        if (first_line_break)
            line[first_line_break] = ' ';

        auto second_line_break = line.find('\n');
        if (second_line_break)
            line = line.substr(0,second_line_break);

        PLOGE << (type==1?"vertex: ":"fragment: ") << line;
        PLOGV <<source;

    }

    return success;

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { destroy();

    if (owned)
        delete builder_v;

}

ShaderProgram::ShaderProgram() { Builder builder; builder.build(); create(builder.frag(),builder.vert()); builder.post(); }

ShaderProgram::ShaderProgram(std::string frag, std::string vert) { create(frag,vert); }

void ShaderProgram::destroy() {

    loaded = false;
    if (id > -1)
        glDeleteProgram(id);

}


Builder* ShaderProgram::builder() {

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

void  ShaderProgram::create(Builder* builder) { builder->build(); create(builder->frag(), builder->vert()); builder->post(); }

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

    // sendUniform("atlas_pass", 1);

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
