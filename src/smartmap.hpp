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

    static inline UBO *matriceUBO;
    static inline UBO *fixtureUBO; 
    static inline ShaderProgram *shader;

    struct Layer {

        enum Mode { Free, Grid };
    
        static inline std::vector<Layer*> pool;

        Fixture fixture;
 
        Mode mode;

        unsigned int width, height;
        unsigned int offset_x, offset_y;

        unsigned int quantity_x, quantity_y, quantity;

        float* output = nullptr;

        Texture *buffer, *pass;
        FrameBuffer *fb;

        Layer(uint16_t chan, uint16_t uni, Fixture fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1) 
            : fixture(fixture), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y) {

            pool.push_back(this);

            auto FW = width*quantity_x*scale;
            auto FH = height*quantity_y*scale;
            buffer = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            pass = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            fb = new FrameBuffer(buffer);

            std::vector<std::array<float, 4>> mat = matrice(quantity_x,quantity_y);    
            matriceUBO->update(&mat[0][0],mat.size()*32); 
            shader->sendUniform("MatriceUBOSize", quantity_x*quantity_y);

        }

    };

    std::vector<float> debuguniforms{0,0,0,0,0,0,0,0,0,0};

    bool debug = true;

    Artnet *artnet;

    Window *window;

    VBO  *quad, *quadA, *quadB;

    FrameBuffer *winFB;

    // ShaderProgram *blur_x, *blur_y;

    GUI *gui;

    Atlas *atlas;

    float time,fps;

    void createLayer(uint16_t chan, uint16_t uni, Fixture *fixture, uint16_t width, uint16_t height, Layer::Mode mode = Layer::Mode::Free, uint16_t quantity_x=1, uint16_t quantity_y=1);

    static SmartMap& getInstance();

    void render();

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};
