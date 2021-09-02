#ifndef SHADER_H
#define SHADER_H

#include "globals.hpp"

enum ShaderType { FRAGMENT, VERTEX, COMPUTE };

struct Shader {


  GLuint id;

  GLenum type;

  Shader(std::string file);

  ~Shader();

  void compile();
  
  operator GLuint();

};

struct ShaderProgram {

  GLuint id;

  ~ShaderProgram();

  ShaderProgram();

  ShaderProgram(const char *vertexShader, const char *fragmentShader);

  ShaderProgram(const char *computeShader);

  void init();

  void load(const char *vertexShader, const char *fragmentShader);

  void build();

  void compile(GLuint fragment, GLuint vertex);

  void compile(GLuint compute);

  void use();

  int getLoc(const std::string& name);
  void sendUniform(const std::string& name, int i1);
  void sendUniform(const std::string& name, float f1);
  void sendUniform(const std::string& name, float f1, float f2);
  void sendUniform(const std::string& name, float f1, float f2, float f3);
  void sendUniform(const std::string& name, float f1, float f2, float f3, float f4);

  operator GLuint();

};

#endif
