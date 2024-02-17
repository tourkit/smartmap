#pragma once

#include <map>
#include <vector>
#include <string>

struct File;

struct Effector   {

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    File * file;
    
    Effector();

    Effector(File *file);

    void import(File *file);

    std::vector<std::string> args;

    std::vector<std::string> extractArgsFromFunction(const std::string& functionSrc);
    
};