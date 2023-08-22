#pragma once

#include "engine.hpp"

#include "atlas.hpp"
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

    ShaderProgram shader;

    VBO quad; 

    Atlas atlas;

    UBO matriceUBO("matriceUBO");
    UBO matriceUBO2("matriceUBO2");
    UBO fixtureUBO("fixtureUBO"); 
    UBO fixtureUBO2("fixtureUBO2"); 

    // Artnet artnet;
    DMX::Fixture basic_fixture;

    struct Layer {

        // static inline std::vector<Layer*> pool;

        // enum Mode { Free, Grid } mode;

        // uint16_t width, height;
        
        // uint16_t offset_x = 0, offset_y =0;

        // uint16_t quantity_x, quantity_y, quantity;

        // Texture *buffer, *pass, *FTbuffer;

        // FrameBuffer *fb;

        // VBO *quadA, *quadB;

        // std::vector<uint8_t> black;

        // uint16_t chan,uni,attroffset = 0, matoffset = 0;

        // Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1);

    };

};
