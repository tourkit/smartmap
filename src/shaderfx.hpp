#pragma once


#include "node.hpp"
#include "file.hpp"
#include "shader.hpp"

struct ShaderFX : Node {

    ShaderType type;

    std::vector<float> uniforms;

    std::string code;

    std::string filename;

    ShaderFX(std::string filename) { import(filename); }

    void import(std::string filename) {

        File file(REPO_DIR+"assets/shaders/"+filename);

        //split here

        this->filename = filename;
        this->name = filename;

        code = file.data;

    }
    
};

