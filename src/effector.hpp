#pragma once

#include <map>
#include <vector>
#include <string>

struct File;
struct Struct;

struct Effector  {

    static inline std::vector<Effector*> pool;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    File * file = nullptr;

    Struct* s = nullptr;

    std::string source();

    Effector(File *file = nullptr);

    std::vector<std::pair<std::string,std::string>> args;

    std::map<std::string, float[3]> ranges;

    static Effector* get(File * file);


};
