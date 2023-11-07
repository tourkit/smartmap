#pragma once
 
#include "../framebuffer.hpp"
#include "../texture.hpp"
#include "../vbo.hpp"
#include "../DMX.hpp"

namespace SmartMap {

    struct Layer {

        static inline std::vector<Layer*> pool;

        enum Mode { Free, Grid } mode;

        uint16_t width, height, id;
        
        uint16_t offset_x = 0, offset_y =0;

        uint16_t quantity_x, quantity_y, quantity;

        Texture *pass, *FTbuffer;

        FrameBuffer *fb;

        const char* chars =  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,!?'\":()[]<>-_+*/\\|#@&%$€";
        float char_ratio; 

        VBO *quad;

        std::vector<uint8_t> black;

        uint16_t chan,uni, fixture_first, attroffset = 0, matoffset = 0;

        Layer(uint16_t chan, uint16_t uni, DMX::Fixture &fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1, int output = 1);


    };

};
