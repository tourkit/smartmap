#pragma once

#include <map>
#include <vector>
#include <string>

struct File;

#include "struct.hpp"

struct Effector : Struct  {

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    File * file;

    std::string source();

    Effector(File *file = nullptr);
    
    std::vector<std::pair<std::string,std::string>> args;

    std::map<std::string, float[3]> ranges;
    
};