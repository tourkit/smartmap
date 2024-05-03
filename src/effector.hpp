#pragma once

#include "struct.hpp"

#include <map>
#include <vector>
#include <string>

struct File;
struct Struct;

struct Effector  {

    struct Definition {

        Struct s;

        std::vector<std::pair<std::string,std::string>> args;

        std::map<std::string, float[3]> ranges;

    };

    static std::string source(File* file);

    static inline std::map<File*, Definition> effects;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    static void init(File* f);

    //     effects[f] = Struct(f->name_v.c_str());

    //     effects[f].add<float>("x").range(0,1,0);
    //     effects[f].add<float>("y").range(0,1,0);

    // }

    static Definition& get(File* f) ;

    Ref ref;

    File* file;

    Effector(File* f, std::string name );


};
