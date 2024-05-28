#pragma once

#include "struct.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "engine.hpp"

struct UberLayer {

    Struct layers_def;

    Instance* glsl_struct;

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
