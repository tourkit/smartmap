# pragma once



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

        static inline SmartMap* sm;

        Layer(uint16_t chan, uint16_t uni, Fixture fixture, uint16_t width, uint16_t height, Layer::Mode mode, uint16_t quantity_x, uint16_t quantity_y, float scale = 1) 
            : fixture(fixture), width(width), height(height), mode(mode), quantity_x(quantity_x), quantity_y(quantity_y), quantity(quantity_x*quantity_y) {

            pool.push_back(this);

            auto FW = width*quantity_x*scale;
            auto FH = height*quantity_y*scale;
            buffer = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            pass = new Texture(nullptr, FW, FH, 0,0,0,GL_RGBA8);
            fb = new FrameBuffer(buffer);

            std::vector<std::array<float, 4>> mat = matrice(quantity_x,quantity_y);    
            sm->matriceUBO->update(&mat[0][0],mat.size()*32); 
            sm->shader->sendUniform("MatriceUBOSize", quantity_x*quantity_y);

            
        }

    };