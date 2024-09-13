#include "shader.hpp"
#include "atlas.hpp"
#include "builder.hpp"

#include <GL/gl3w.h>
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <chrono>
#include <regex>

#include <string>

#include <boost/algorithm/string.hpp>

Shader::Shader() { }

Shader::~Shader() { if (id > -1) glDeleteShader(id);  }

Shader::Shader(std::string src, uint8_t type)  { create(src,type); }


struct ShaderError {

    std::vector<int> coords  = {0,0,0};
    std::string val;

    bool operator==(ShaderError& other) { 

        return val == other.val && coords[0] == other.coords[0] && coords[1] == other.coords[1] && coords[2] == other.coords[2];
    }

};

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
        std::vector<ShaderError> errors;
    
        std::istringstream iss(infoLog);

        for (std::string line; std::getline(iss, line); ) {
            
            std::smatch match;
            std::regex color_regex(".?([0-9]+):([0-9]+)(\\(([0-9]+)\\): error)?: (.+)", std::regex_constants::icase);
            ShaderError error;

            if (std::regex_search(line, match, color_regex)) {

                ShaderError error;

                error.coords[0] = std::stoi(match[1].str());
                error.coords[1] = std::stoi(match[2].str());
                if (match.size()>4)
                    error.coords[2] = std::stoi(match[4].str());
                if (match.size()>5)
                    error.val = match[5].str();
                
                bool found = false;
                for (auto e : errors) 
                    if (e == error) {
                        found = true;
                        break;
                    }

                if (found)
                    break;
                else
                    errors.push_back(error);

            }

        }

        for (auto &e : errors) 
            for (auto &e_ : errors) 
                if (&e != &e_ && e.coords == e_.coords) {
                    if (e.val.length())
                        e.val+= ", ";
                    e.val += e_.val;
                    e_.val.clear();
                    break;
                }

        std::string output;

        if (type == FRAGMENT) output += "[fragment] ";
        else if (type == VERTEX) output += "[vertex] ";
        else if (type == COMPUTE) output += "[compute] ";
        output += "";

        for (auto error : errors)

            if (error.val.length()) {

                std::string error_str;

                error_str += std::to_string(error.coords[0]) + ":" + std::to_string(error.coords[1]);
                
                if (error.coords[2])
                    error_str += "("+std::to_string(error.coords[2])+")";

                error_str += ": " + error.val + " && ";

                if (error_str.length())
                    error_str = error_str.substr(0,error_str.length()-4);

                
                iss = std::istringstream (src);

                int count = 0;
                for (std::string line; std::getline(iss, line); ) {
                    if (count++ == error.coords[1]-1) {
                        boost::trim_left(line);
                        error_str += " : " + line;    
                        break;
                        
                    }
                }

                output += error_str;
                    
            }



        PLOGE << output;
        PLOGV << "\n" << source;
        return success;
        iss = std::istringstream (source);
        int count = 1;

        for (std::string line; std::getline(iss, line); ) {

            PLOGV << count++ << ": " << line;

        }

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
