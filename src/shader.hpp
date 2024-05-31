#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <map>
#include <string>

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

  void use();

  void use(uint32_t x, uint32_t y = 1, uint32_t z = 1);

  void destroy();
  void create(std::string frag, std::string vert);

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

    static inline std::string version = "#version 430 core\n\n";

    std::string header_common, header_fragment , header_vertex, body_fragment , body_vertex, fragment, vertex  ;

    virtual void build();

    virtual void frag();

    virtual void vert();

    std::string layout(UBO* ubo);

    std::string define(Member* member);

    std::map<Member*,std::string> list;

  };

  static inline Builder default_builder;

  Builder* builder = &default_builder;

  void create();

  void create(Builder* builder);

};
