#pragma once


// #include "shader.hpp"
#include <map>
#include <vector>
#include <string>

struct File;

struct ShaderFX   {

    // Shader::Type type;

    enum Type { FRAGMENT, VERTEX, COMPUTE } type;

    std::map<std::string, float> uniforms;

    File * file;
    
    ShaderFX();
    ShaderFX(File *file);

    void import(File *file);

    std::vector<std::string> args;

    std::vector<std::string> extractArgsFromFunction(const std::string& functionSrc);
    
};