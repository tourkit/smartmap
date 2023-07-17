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

struct Quads {

    Quads() { vbo.destroy(); }

    std::vector<uint16_t> list;

    VBO vbo;

    void add(uint16_t id,uint16_t width,uint16_t height) { 
        
        list.push_back(id); 

        vbo.import("quad.obj",id,width, height); 
        
    }

    void remove(uint16_t id) {  

        for (int i = 0; i < list.size(); i++) if (list[i] == id) {list.erase(list.begin()+i); break;}

        vbo.destroy(); 

        for (auto id:list) vbo.import("quad.obj",id); 
        
        vbo.update(); 
        
    }

    void draw(uint16_t quantity) { vbo.draw(quantity); }

};

struct SmartMap {

    using Fixture = std::vector<Artnet::Universe::Attribute>;

    static inline Artnet *artnet;
    static inline UBO *matriceUBO;
    static inline UBO *fixtureUBO; 
    static inline UBO *fixtureUBO2; 
    static inline ShaderProgram *shader;

    struct Layer {

        static inline std::vector<Layer*> pool;

        enum Mode { Free, Grid } mode;
    
        Fixture fixture;

        uint16_t width, height;
        
        uint16_t offset_x = 0, offset_y =0;

        uint16_t quantity_x, quantity_y, quantity;

        Texture *buffer, *pass, *FTbuffer;

        FrameBuffer *fb;

        VBO *quadA, *quadB;

        uint16_t chan,uni,attroffset = 0, matoffset = 0;

        Layer(uint16_t chan, uint16_t uni, Fixture& fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1);

    };

    Window *window;

    VBO *quad; 

    FrameBuffer *winFB;

    // Quads* quads1;

    // ShaderProgram *blur_x, *blur_y;

    GUI *gui;

    Atlas *atlas;

    static SmartMap& getInstance();

    void render();

    //////////////////////////////////////////////////////////////////////

    bool debug = true;

    std::vector<float> debuguniforms{0,0,0,0,0,0,0,0,0,0};

    GLint GL_BLEND_MODE_IN = 11;
    GLint GL_BLEND_MODE_OUT = 13;
    GLint GL_BLEND_MODE_IN2 = 2;
    GLint GL_BLEND_MODE_OUT2 = 1;
    GLint GL_BLEND_MODE_IN3 = 2;
    GLint GL_BLEND_MODE_OUT3 = 1;
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
        // GL_SRC1_COLOR,
        // GL_ONE_MINUS_SRC1_COLOR,
        // GL_SRC1_ALPHA,
        // GL_ONE_MINUS_SRC1_ALPHA

    };

    //////////////////////////////////////////////////////////////////////

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};
