#pragma once

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "atlas.hpp"
#include "ubo.hpp"
#include "gui.hpp"
#include "artnet.hpp"


struct SmartMap {

    using Fixture = std::vector<Artnet::Universe::Attribute>;

    GLint GL_BLEND_MODE_IN = 1;
    GLint GL_BLEND_MODE_OUT = 13;
    std::vector<GLenum> GL_BLEND_MODES = {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,	
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA,
        GL_SRC_ALPHA_SATURATE,
        GL_SRC1_COLOR,
        GL_ONE_MINUS_SRC1_COLOR,
        GL_SRC1_ALPHA,
        GL_ONE_MINUS_SRC1_ALPHA

    };

    static inline Artnet *artnet;
    static inline UBO *matriceUBO;
    static inline UBO *fixtureUBO; 
    static inline ShaderProgram *shader;

    struct Layer {

        static inline std::vector<Layer*> pool;

        enum Mode { Free, Grid } mode;
    
        Fixture fixture;

        unsigned int width, height;
        
        unsigned int offset_x = 0, offset_y =0;

        unsigned int quantity_x, quantity_y, quantity;

        Texture *buffer, *pass;

        FrameBuffer *fb;

        uint16_t chan,uni,offset = 0;

        Layer(uint16_t chan, uint16_t uni, Fixture& fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1) 
            : chan(chan), uni(uni), fixture(fixture), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y) {

            for (auto l:pool) { offset+=l->quantity*l->fixture.size(); }
            pool.push_back(this);
            
            auto FW = width*quantity_x*scale;
            auto FH = height*quantity_y*scale;
            buffer = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            pass = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            fb = new FrameBuffer(buffer);

            std::vector<std::array<float, 4>> mat = matrice(quantity_x,quantity_y);    
            matriceUBO->update(&mat[0][0],mat.size()*32); 
            shader->sendUniform("MatriceUBOSize", quantity_x*quantity_y);
            
            artnet->universes[uni].callbacks.push_back([this](Artnet::Universe* u){ u->remap(this->chan, this->quantity ,this->fixture ,&fixtureUBO->data[this->offset]); });

        }

    };

    std::vector<float> debuguniforms{0,0,0,0,0,0,0,0,0,0};

    bool debug = true;

    Window *window;

    VBO  *quad, *quadA, *quadB;

    FrameBuffer *winFB;

    // ShaderProgram *blur_x, *blur_y;

    GUI *gui;

    Atlas *atlas;

    static SmartMap& getInstance();

    void render();

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};
