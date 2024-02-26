#include "shader.hpp"
#include "log.hpp"

#include <GL/gl3w.h>
#include <chrono>

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
        // PLOGD <<source;
        
    }

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { destroy(); }

ShaderProgram::ShaderProgram() { }

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

void ShaderProgram::use(GLuint x, GLuint y, GLuint z) {  glUseProgram(id); glDispatchCompute(x,y,z); }

ShaderProgram::operator GLuint() { return id; }

int ShaderProgram::getLoc(const std::string& name) { return glGetUniformLocation(id, name.c_str()); }

void ShaderProgram::sendUniform(const std::string& name, int i1) { glProgramUniform1i(id, getLoc(name), i1); }

void ShaderProgram::sendUniform(const std::string& name, float f1) { glProgramUniform1f(id, getLoc(name), f1); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2) { glProgramUniform2f(id, getLoc(name), f1, f2); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2, float f3) { glProgramUniform3f(id, getLoc(name), f1, f2, f3); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2, float f3, float f4) { glProgramUniform4f(id, getLoc(name), f1, f2, f3, f4); }

//void Shader::sendUniform(std::string name, glm::mat4 mat) { glProgramUniformMatrix4fv(program, getLoc(name), 1, GL_FALSE, &mat[0][0]); }


