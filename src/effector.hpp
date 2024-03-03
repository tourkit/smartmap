#pragma once

#include <map>
#include <vector>
#include <string>

struct File;
struct Component;

struct Effector   {

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    File * file = nullptr;

    Component* comp = nullptr; // owned

    std::string source();
    
    Effector();

    Effector(File *file);

    Effector(const char* data);

    void import(File *file);
    
    void import(const char* data);

    std::vector<std::pair<std::string,std::string>> args;

    std::map<std::string, float[3]> ranges;
    
};