#pragma once

#include <cstdint>
#include <string>
#include <chrono>

struct UBO;
struct DrawCall;

struct Shader {

  uint32_t id = -1;

  enum Type { FRAGMENT, VERTEX, COMPUTE } type;

  std::string src;

  Shader();

  Shader(std::string src, uint8_t type = 0);

  void create(std::string src, uint8_t type);

  ~Shader();

  void compile();

  operator uint32_t();

};

struct Member;
struct ShaderProgram {

  uint32_t id = -1;

  Shader frag, vert;

  bool loaded = false;

  ~ShaderProgram();

  ShaderProgram();

  ShaderProgram(std::string frag, std::string vert);

  ShaderProgram(DrawCall* dc);

  void use();

  void use(uint32_t x, uint32_t y = 1, uint32_t z = 1);

  void destroy();
  void create(std::string frag, std::string vert);
  void create(DrawCall* dc);

  int getLoc(const std::string& name);
  void sendUniform(const std::string& name, int i1);
  void sendUniform(const std::string& name, float f1);
  void sendUniform(const std::string& name, float f1, float f2);
  void sendUniform(const std::string& name, float f1, float f2, float f3);
  void sendUniform(const std::string& name, float f1, float f2, float f3, float f4);

  operator uint32_t();

  std::chrono::_V2::system_clock::time_point last_change;

  struct Builder {

    static inline std::string struct_taber = "";//\t";
    static inline std::string struct_spacer = " ";//\n\n";
    static inline std::string comment_line  = "///////////////////////////////////////////\n\n";

    std::string header_common = "#version 430 core\n\n";
    std::string layouts, header_fragment , header_vertex , footer_common;

    std::set<File*> effectors; // must be filled by UBO which is not current solution // agreed twice

    void build();
    std::string frag();
    std::string vert();

    DrawCall* dc;

    Builder();
    Builder(DrawCall* dc);

    std::string layout(UBO *ubo);

  private:
    int stride_count = 0;

  };

};
