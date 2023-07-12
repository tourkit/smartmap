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

    struct Layer {

        enum Mode { Free, Grid };
    
        std::vector<Layer*> pool;

        Fixture fixture;
 
        Mode mode;

        unsigned int width, height;
        unsigned int offset_x, offset_y;

        unsigned int quantity_x, quantity_y, quantity;

        float* output = nullptr;

        Texture *buffer, *pass;
        FrameBuffer *fb;

        Layer(GLuint chan, GLuint uni, Fixture fixture, GLuint width, GLuint height, Layer::Mode mode, GLuint quantity_x, GLuint quantity_y, float scale = 1) 
            : fixture(fixture), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y) {

            auto FW = width*quantity_x*scale;
            auto FH = height*quantity_y*scale;
            // buffer = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            // pass = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            // fb = new FrameBuffer(buffer);
            
        }

    };

    std::vector<float> debuguniforms{0,0,0,0,0,0,0,0,0,0};

    bool debug = true;

    Artnet *artnet;

    Window *window;

    VBO  *quad, *quadA, *quadB;

    Texture *passBuf, *outBuf, *outBlur;

    FrameBuffer *outFB, *winFB;

    ShaderProgram *basic, *shader, *blur_x, *blur_y;

    GUI *gui;

    UBO *matriceUBO, *fixtureUBO; 

    uint16_t FW, FH, MAT_X, MAT_Y;

    Atlas *atlas;

    float time,fps;

    void createLayer(GLuint chan, GLuint uni, Fixture *fixture, GLuint width, GLuint height, Layer::Mode mode = Layer::Mode::Free, GLuint quantity_x=1, GLuint quantity_y=1);

    static SmartMap& getInstance();

    void render();

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};
