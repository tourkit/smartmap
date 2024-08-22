#pragma once

#include "struct.hpp"
#include "effector.hpp"

#include "RectangleBinPack/MaxRectsBinPack.h"
#include <string>

struct Texture;
struct UBO; 
struct Struct; 
struct ShaderProgram; 
struct Instance; 
 
struct Atlas {

    Member m;

    rbp::MaxRectsBinPack binpack;

    Texture *texture;
         
    UBO *ubo; 

    Instance* medias;

    std::string path;

    Atlas(int width = 4096, int height = 4096 , std::string path = "");

    void clear();

    void fromDir(std::string path);

    void link(ShaderProgram* shader);

    struct Effector : ::Effector {

        Atlas* atlas;

        Effector(Atlas* atlas = nullptr);

        bool setup(Builder* builder) override;
        
        void post(Builder* builder) override;

        std::string source() override;

    } effector;
};
