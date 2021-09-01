#include "shader.hpp"

std::string readFile(const char* path) {

  std::string data;

  std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
  file.seekg(0, std::ios::end);
  std::streampos length(file.tellg());

  if (length) {

      file.seekg(0, std::ios::beg);
      data.resize(length);
      file.read(&data[0], length);

  }

  file.clear();

  return data;

}

namespace GL {

Shader::Shader(ShaderType type) : type(type) {  }

ShaderProgram::ShaderProgram() { id = 0; }

ShaderProgram::~ShaderProgram() { if (id) glDeleteProgram(id); }

ShaderProgram::ShaderProgram(const char *vertexShader, const char *fragmentShader) { load(vertexShader, fragmentShader); }

ShaderProgram::ShaderProgram(const char *computeShader) {

    id = glCreateProgram();

    compute.code = readFile(computeShader);

    compute.compile();

    compile(compute.id);
  
}

void ShaderProgram::load(const char *vertexShader, const char *fragmentShader) {

        id = glCreateProgram();

        

    fragment.code = readFile(fragmentShader);

    vertex.code = readFile(vertexShader);

    fragment.compile();
    vertex.compile();

    compile(fragment.id, vertex.id);

}


void Shader::error(GLuint type, std::string out) {

    GLchar infoLog[512];
    GLint success;

    glGetShaderiv(id, type, &success);

    if (!success) {

        glGetShaderInfoLog(id, 512, NULL, infoLog);

        std::string logstr = infoLog;

        std::cout << out << logstr;
        
    }

}

void Shader::compile() {

    if (type == FRAGMENT) id = glCreateShader(GL_FRAGMENT_SHADER);
    else if (type == VERTEX) id = glCreateShader(GL_VERTEX_SHADER);
    else if (type == COMPUTE) id = glCreateShader(GL_COMPUTE_SHADER);
    else std::cout << "shadertype error in Shader::compile" << std::endl;

    auto ptr = code.data(); // J'arive pas a juste ecire (GLchar* const*)code.data() dans glShaderSource a la place de &ptr ca m'eneeeeeerve
    glShaderSource(id, 1, &ptr, nullptr);

    glCompileShader(id);

    std::string strtype;
    if (type == FRAGMENT) strtype = "frag";
    else if (type == VERTEX) strtype = "vert";
    else if (type == COMPUTE)strtype = "comp" ;

    error(GL_COMPILE_STATUS, strtype+": ");

}
void Shader::build() {

    int count = 0;

    compile();

}

void ShaderProgram::init() {

    id = glCreateProgram();

}


void ShaderProgram::build() {

    fragment.build();
    vertex.build();

    compile(fragment.id, vertex.id);

}

void ShaderProgram::compile(GLuint fragment, GLuint vertex) {

    glAttachShader(id, fragment);
    glAttachShader(id, vertex);

    glLinkProgram( id );

    glDeleteShader( fragment ); 
    glDeleteShader( vertex );

}

void ShaderProgram::compile(GLuint compute) {

    glAttachShader(id, compute);

    glLinkProgram( id );

    glDeleteShader( compute );

}

void ShaderProgram::use() {  glUseProgram(id); }

ShaderProgram::operator GLuint() { return id; }

int ShaderProgram::getLoc(const std::string& name) { return glGetUniformLocation(id, name.c_str()); }

void ShaderProgram::sendUniform(const std::string& name, int i1) { glProgramUniform1i(id, getLoc(name), i1); }

void ShaderProgram::sendUniform(const std::string& name, float f1) { glProgramUniform1f(id, getLoc(name), f1); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2) { glProgramUniform2f(id, getLoc(name), f1, f2); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2, float f3) { glProgramUniform3f(id, getLoc(name), f1, f2, f3); }

void ShaderProgram::sendUniform(const std::string& name, float f1, float f2, float f3, float f4) { glProgramUniform4f(id, getLoc(name), f1, f2, f3, f4); }

//void Shader::sendUniform(std::string name, glm::mat4 mat) { glProgramUniformMatrix4fv(program, getLoc(name), 1, GL_FALSE, &mat[0][0]); }



}
