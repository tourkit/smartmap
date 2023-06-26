#pragma once

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "atlas.hpp"
#include "ubo.hpp"
#include "gui.hpp"
#include "fixture.hpp" 
#include "device/artnet.hpp" 

 
struct SmartMap {

    Artnet artnet;

    GL::Window window;

    VBO quad;
    Texture tex;
    ShaderProgram shader;

    GUI gui;

    static SmartMap& getInstance();

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};

extern SmartMap& sm;