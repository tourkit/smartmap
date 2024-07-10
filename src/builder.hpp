#pragma once

#include <string>
#include <map>
#include <vector>

struct Member;
struct UBO;
struct VBO;
struct ShaderProgram;
struct Effector;

struct Builder {

    static inline std::string struct_taber = "";//\t";

    static inline std::string struct_spacer = " ";//\n\n";

    static inline std::string comment_line  = "///////////////////////////////////////////\n\n";

    static inline std::string version = "#version 430 core\n\n";

    std::string header_common, header_fragment , header_vertex, body_fragment , body_vertex, effectors_vertex_str , current_model, current_layer;

    std::vector<Effector*> effectors_fragment, effectors_vertex;

    std::vector<UBO*> ubos;

    std::map<int,std::string> samplers;

    void addSampler(std::string name);

    virtual void build();

    std::string frag();

    std::string vert();

    std::string layout();

    bool add(UBO* ubo);

    VBO* vbo = nullptr;

    ShaderProgram* shader = nullptr;

    struct VBOLayout { int loc; std::string type,name;};

    std::vector<VBOLayout> vbo_layouts;

    std::map<Member*,std::string> list; // toremove

private:

    std::string print_struct(Member* member, std::map<Member*,std::string> &unique_name_list);

  };
