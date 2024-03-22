#pragma once

#include "RectangleBinPack/MaxRectsBinPack.h"
#include <string>

struct Texture;
struct UBO; 
struct Struct; 
struct ShaderProgram; 
struct Instance; 
 
struct Atlas {

    Struct* buffer;

    rbp::MaxRectsBinPack binpack;

    Texture *texture;
         
    UBO *ubo; 

    Instance* medias;

    std::string path;

    Atlas(int width = 4096, int height = 4096 , std::string path = "");

    void clear();

    void fromDir(std::string path);

    void link(ShaderProgram* shader);

};