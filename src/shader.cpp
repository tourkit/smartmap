#include "shader.hpp"

Shader::~Shader() { glDeleteShader(id);  }

Shader::Shader(std::string file) {

    std::string ext = file.substr(file.find_first_of(".")+1);

    if (ext == "frag") type = GL_FRAGMENT_SHADER;
    else if (ext == "vert") type = GL_VERTEX_SHADER;
    else if (ext == "comp") type = GL_COMPUTE_SHADER;
    else std::cout << "shadertype error in Shader::compile" << std::endl;

    file = readFile(file.c_str());

    id = glCreateShader(type);
 
    auto ptr = file.data(); // J'arive pas a juste ecire (const GLchar* const*)code.data() dans glShaderSource a la place de &ptr ca m'eneeeeeerve
    glShaderSource(id, 1, &ptr, nullptr);
 
    glCompileShader(id);


    // Check for errors

    GLchar infoLog[512];
    GLint success;

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(id, 512, NULL, infoLog);

        GL_PRINT(ext << ": " << infoLog);
        
    }

}

Shader::operator GLuint() { return id; }

ShaderProgram::~ShaderProgram() { if (id) glDeleteProgram(id); }

ShaderProgram::ShaderProgram(std::vector<std::string> paths) {

    id = glCreateProgram();

    std::set<std::shared_ptr<Shader>> shaders;

    for (auto p:paths) shaders.insert(std::make_shared<Shader>(p));

    for (auto shader:shaders) glAttachShader(id, shader->id);

    glLinkProgram( id );

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


