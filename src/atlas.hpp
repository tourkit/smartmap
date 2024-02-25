#pragma once

#include "RectangleBinPack/MaxRectsBinPack.h"
#include <string>

struct Texture;
struct UBO; 
struct Object; 
struct ShaderProgram; 
 
struct Atlas {

    static inline Object* buffer = nullptr;

    rbp::MaxRectsBinPack binpack;

    std::vector<std::array<float,4>> normalized_list;

    Texture *texture;
         
    UBO *ubo; 

    std::string path;

    Atlas(int width = 4096, int height = 4096 , std::string path = "");

    void clear();

    void fromDir(std::string path);

    void link(ShaderProgram* shader);

};