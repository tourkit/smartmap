#include "shader.hpp"
#include "engine.hpp"
// #include "pch.hpp"

#include <filesystem>

namespace fs = std::filesystem;

Shader::~Shader() { if (id) glDeleteShader(id);  }

Shader::Shader(std::string path)  {

    file.read("assets/shader/" + path);

    if (file.extension == "frag") type = GL_FRAGMENT_SHADER;
    else if (file.extension == "vert") type = GL_VERTEX_SHADER;
    else if (file.extension == "comp") type = GL_COMPUTE_SHADER;
    else std::cout << "SMARTMAP FILENAME ERROR" << std::endl;

    id = glCreateShader(type);
 
    auto ptr = file.data.c_str(); // J'arive pas a juste ecire (const GLchar* const*)code.data() dans glShaderSource a la place de &ptr ca m'eneeeeeerve
    glShaderSource(id, 1, &ptr, nullptr);
 
    glCompileShader(id);

    // Check for errors

    GLchar infoLog[512];
    GLint success;

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(id, 512, NULL, infoLog);

        GL_PRINT(infoLog);
        
    }

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { destroy(); }


ShaderProgram::ShaderProgram(std::vector<std::string> paths, bool surveying) : paths(paths) {

    ShaderProgram::pool.push_back(this);

    create();

    use();

}

void ShaderProgram::destroy() {  // add pool mgmt
    loaded = false;
    if (id) glDeleteProgram(id); 
    shaders.resize(0);
}

void ShaderProgram::reset() {  

    destroy();
    create();
    
}

  void  ShaderProgram::create() {  

    id = glCreateProgram();

    for (auto p:paths) shaders.push_back(std::make_unique<Shader>(p));

    for (auto &shader:shaders) {
        
        glAttachShader(id, shader->id);
    
        shader->file.survey = true;
        shader->file.callback = [this](File* file){ file->update(); this->reset(); this->sendUniform("mediasAtlas", 1); this->sendUniform("freetype", 2); };
    
    }

    glLinkProgram( id );

    engine.dynamic_ubo.subscribers.push_back(this); 
    engine.static_ubo.subscribers.push_back(this); 

    loaded = true;

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


