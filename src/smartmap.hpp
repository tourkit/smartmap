#pragma once

#include "engine.hpp"

#include "atlas.hpp"
#include "artnet.hpp"



struct SmartMap {


    static inline Atlas *atlas;

    SmartMap();


    static inline Artnet *artnet;
    static inline ShaderProgram *shader;
    
    DMX::Fixture basic_fixture;

    static inline Buffer::Object *fix1UBO;
    static inline Buffer::Object *fix2UBO;
    static inline Buffer::Object *mat1UBO;
    static inline Buffer::Object *mat2UBO;

    struct Layer {

        static inline std::vector<Layer*> pool;

        enum Mode { Free, Grid } mode;

        uint16_t width, height;
        
        uint16_t offset_x = 0, offset_y =0;

        uint16_t quantity_x, quantity_y, quantity;

        Texture *buffer, *pass, *FTbuffer;

        FrameBuffer *fb;

        VBO *quadA, *quadB;

        std::vector<uint8_t> black;

        uint16_t chan,uni,attroffset = 0, matoffset = 0;

        Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1);

    };

    struct Widget : GUI::Window {

        Widget() : GUI::Window("VIEW") { }

        void draw() override {

            for (int i = 0; i < Texture::pool.size(); i++) {

                std::string wh = std::to_string(Texture::pool[i]->width) + " x " + std::to_string(Texture::pool[i]->height);
                ImGui::Text(wh.c_str());
                
                float ratio = Texture::pool[i]->height/(float)Texture::pool[i]->width;
                auto nw = std::min(Texture::pool[i]->width,(GLuint)512);

                ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)Texture::pool[i]->id, ImVec2(nw,nw*ratio));

                ImGui::PushID(i+100);

                ImGui::PopID();
                ImGui::Separator();
            }
            
        }

    } widget;


    


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
