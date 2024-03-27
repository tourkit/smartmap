#pragma once

#include <map>
#include <vector>
#include <string>

struct File;
struct Struct;

struct Effector  {

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    File * file;

    Struct* s;

    std::string source();

    Effector(File *file = nullptr);

    std::vector<std::pair<std::string,std::string>> args;

    std::map<std::string, float[3]> ranges;

};
