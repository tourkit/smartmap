#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>

struct Builder;
struct Member;


struct Shader {

  typedef std::map<int, std::string> Errors;

  uint32_t id = -1;

  enum Type { FRAGMENT, VERTEX, COMPUTE } type;

  std::string src;

  Shader();

  Shader(std::string src, uint8_t type = 0);

  bool create(std::string src, uint8_t type);

  ~Shader();

  void compile();

  operator uint32_t();

  char infoLog[512];

  Errors errors;

};

struct ShaderProgram {

  uint32_t id = -1;

  Shader frag, vert;

  bool loaded = false;

  ~ShaderProgram();
  
  ShaderProgram();

  ShaderProgram(std::string frag, std::string vert);

  void use();

  void use(uint32_t x, uint32_t y = 1, uint32_t z = 1);

  void destroy();
  
  virtual void create(std::string frag, std::string vert);

  int getLoc(const std::string& name);
  void sendUniform(const std::string& name, int i1);
  void sendUniform(const std::string& name, float f1);
  void sendUniform(const std::string& name, float f1, float f2);
  void sendUniform(const std::string& name, float f1, float f2, float f3);
  void sendUniform(const std::string& name, float f1, float f2, float f3, float f4);

};
