#ifndef ATLAS_H
#define ATLAS_H  
#include "RectangleBinPack/GuillotineBinPack.h"
#include "globals.hpp"
#include "texture.hpp"
#include "ubo.hpp"

struct UBO;
 
struct Atlas : public rbp::GuillotineBinPack{

    int width, height;

    std::vector<unsigned char> data;

    std::vector<rbp::Rect> &list = GetUsedRectangles();

    std::vector<std::array<float,4>> normalized_list;

    Texture texture;
         
    UBO ubo; 

    bool buffered = false;

    Atlas(int width = 4096, int height = 4096);

    void init();

    Atlas(std::string path, int width = 4096, int height = 4096);

    bool add(int width, int height, unsigned char* data);

    void link(ShaderProgram* shader);

};


#endif