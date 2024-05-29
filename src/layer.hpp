#pragma once

#include "struct.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "engine.hpp"
#include "drawcall.hpp"

struct UberLayer : Layer {

    Struct layers_def;

    Instance* glsl_layers;

    FrameBuffer fb;

    ShaderProgram shader;

    VBO quad;

    struct VLayer {

        int w ;
        int h;
        int id = 0;
    };

    std::vector<VLayer> layers;

    int w ; int h;

    GLint max_tex_size;

    UberLayer() ;

    void calc(VBO* vbo) ;

    void add(int w , int h) ;

     void draw() ;

};
