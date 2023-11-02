#pragma once

#include "engine.hpp"

#include "atlas.hpp"
#include "artnet.hpp"

struct SmartMap {

    static inline Atlas *atlas;

    SmartMap();

    static inline Artnet *artnet;
    static inline ShaderProgram *shader, *layershader, *basicshader;
    
    DMX::Fixture basic_fixture;

    static inline Buffer::Object *fix1UBO;
    static inline Buffer::Object *fix2UBO;
    static inline Buffer::Object *smartlayersUBO;

    void import(std::string filepath); 

    Stack stack;
    
    struct Layer {

        static inline std::vector<Layer*> pool;

        enum Mode { Free, Grid } mode;

        uint16_t width, height, id;
        
        uint16_t offset_x = 0, offset_y =0;

        uint16_t quantity_x, quantity_y, quantity;

        Texture *pass, *FTbuffer;

        FrameBuffer *fb;

        VBO *quad;

        std::vector<uint8_t> black;

        uint16_t chan,uni, fixture_first, attroffset = 0, matoffset = 0;

        Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1);


    };


    GLint GL_BLEND_MODE_IN = 11;
    GLint GL_BLEND_MODE_OUT = 13;
    GLint GL_BLEND_MODE_IN2 = 1;
    GLint GL_BLEND_MODE_OUT2 = 1;
    GLint GL_BLEND_MODE_IN3 = 1;
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


};




// struct Quads {

//     Quads() { vbo.destroy(); }

//     std::vector<uint16_t> list;

//     VBO vbo;

//     void add(uint16_t id,uint16_t width,uint16_t height) { 
        
//         list.push_back(id); 

//         vbo.import("quad.obj",id,width, height); 
        
//     }

//     void remove(uint16_t id) {  

//         for (int i = 0; i < list.size(); i++) if (list[i] == id) {list.erase(list.begin()+i); break;}

//         vbo.destroy(); 

//         for (auto id:list) vbo.import("quad.obj",id); 
        
//         vbo.update(); 
        
//     }

//     void draw(uint16_t quantity) { vbo.draw(quantity); }

// };
