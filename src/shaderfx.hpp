#pragma once


#include "node.hpp"
#include "shader.hpp"
#include "file.hpp"
#include <map>

struct ShaderFX : Ptr<File>  {

    Shader::Type type;

    std::map<std::string, float> uniforms;

    std::string code;
    
    ShaderFX(File *file);

    void convert();

    void editor() override;

    std::vector<std::string> args;

    std::vector<std::string> extractArgsFromFunction(const std::string& functionSrc);
    
};

struct ShaderFXPtr : Ptr<ShaderFX> {

    ShaderFXPtr(Node* ptr);

    void editor() override;

};
