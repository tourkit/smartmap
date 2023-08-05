#ifndef SHADER_H
#define SHADER_H

#include "pch.hpp"

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

  static inline std::vector<ShaderProgram*> pool;

  GLuint id;

  std::vector<std::string> paths;

  ~ShaderProgram();

  ShaderProgram();

  ShaderProgram(std::vector<std::string> paths);

  void use();
  void use(GLuint x, GLuint y = 1, GLuint z = 1);

  void reset();
  void destroy();
  void create();

  int getLoc(const std::string& name);
  void sendUniform(const std::string& name, int i1);
  void sendUniform(const std::string& name, float f1);
  void sendUniform(const std::string& name, float f1, float f2);
  void sendUniform(const std::string& name, float f1, float f2, float f3);
  void sendUniform(const std::string& name, float f1, float f2, float f3, float f4);

  operator GLuint();

};

#endif
