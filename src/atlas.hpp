#ifndef ATLAS_H
#define ATLAS_H  
#include "RectangleBinPack/MaxRectsBinPack.h"
#include "pch.hpp"
#include "texture.hpp"
#include "ubo.hpp"

struct UBO;
 
struct Atlas {

    rbp::MaxRectsBinPack binpack;

    std::vector<std::array<float,4>> normalized_list;

    Texture *texture;
         
    UBO *ubo; 

    Atlas(std::string path, int width = 4096, int height = 4096);

    void link(ShaderProgram* shader);

};


#endif