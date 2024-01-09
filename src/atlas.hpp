#ifndef ATLAS_H
#define ATLAS_H  
#include "RectangleBinPack/MaxRectsBinPack.h"
#include "pch.hpp"
#include "texture.hpp"
#include "ubo.hpp"
#include "node.hpp"

struct UBO;
 
struct Atlas : Node {

    rbp::MaxRectsBinPack binpack;

    std::vector<std::array<float,4>> normalized_list;

    std::vector<Image> image_list;

    Texture *texture;
         
    UBO *ubo; 

    Atlas(int width = 4096, int height = 4096 , std::string path = "");

    void fromDir(std::string path);

    void link(ShaderProgram* shader);

};


#endif