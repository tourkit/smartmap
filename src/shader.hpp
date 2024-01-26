#ifndef SHADER_H
#define SHADER_H

#include "pch.hpp"
#include "file.hpp"




enum ShaderType { FRAGMENT, VERTEX, COMPUTE };

struct Shader {

  GLuint id = -1;

  GLenum type;

  std::string src;

  Shader();
  
  Shader(std::string src, uint8_t type = 0);

  void create(std::string src, uint8_t type);

  ~Shader();

  void compile();
  
  operator GLuint();

};

struct ShaderProgram {

  GLuint id = -1;

  Shader frag, vert;

  bool loaded = false;

  ~ShaderProgram();

  ShaderProgram();

  ShaderProgram(std::string frag, std::string vert);

  void use();
  void use(GLuint x, GLuint y = 1, GLuint z = 1);

  void reset();
  void destroy();
  void create(std::string frag, std::string vert);

  int getLoc(const std::string& name);
  void sendUniform(const std::string& name, int i1);
  void sendUniform(const std::string& name, float f1);
  void sendUniform(const std::string& name, float f1, float f2);
  void sendUniform(const std::string& name, float f1, float f2, float f3);
  void sendUniform(const std::string& name, float f1, float f2, float f3, float f4);

  operator GLuint();

};

#endif
