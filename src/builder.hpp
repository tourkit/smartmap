#pragma once

#include <string>
#include <map>
#include <vector>
#include "shader.hpp"

struct Member;
struct UBO;
struct VBO;
struct Texture;
struct Effector;

struct Builder {

    ShaderProgram program;

    static inline std::string struct_taber = "";//\t";

    static inline std::string struct_spacer = " ";//\n\n";

    static inline std::string comment_line  = "///////////////////////////////////////////\n\n";

    static inline std::string version = "#version 430 core\n\n";

    std::string header_common, header_fragment , header_vertex, body_fragment , body_vertex, effectors_vertex_str , current_model, current_layer;

    std::vector<Effector*> effectors_fragment, effectors_vertex;

    std::vector<UBO*> ubos;

    std::map<int,Texture*> samplers;

    int addSampler(Texture* tex, std::string name = "");

    virtual void build();
    
    virtual void post();

    std::string frag();

    std::string vert();

    std::string layout();

    bool add(UBO* ubo);

    VBO* vbo = nullptr;

    struct VBOLayout { int loc; std::string type,name;};

    std::vector<VBOLayout> vbo_layouts;

    std::map<Member*,std::string> list; // toremove

    static std::vector<std::pair<Member*,std::string>> unique_name(std::vector<UBO*> ubos);
    static std::string print_struct(Member* member, std::vector<std::pair<Member*,std::string>>& list);

  };
