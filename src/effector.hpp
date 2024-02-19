#pragma once

#include <map>
#include <vector>
#include <string>

struct File;

struct Effector   {

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    File * file;

    std::string source();
    
    Effector();

    Effector(File *file);

    void import(File *file);

    std::vector<std::pair<std::string,std::string>> args;

    std::map<std::string, float[3]> ranges;
    
};