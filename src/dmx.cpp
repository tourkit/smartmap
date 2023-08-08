#include "dmx.hpp"

DMX::DMX() { memset(&data[0],0,512); } 

uint16_t DMX::get16(uint16_t i) { return ((data[i] << 8) | data[i+1]);  }
uint32_t DMX::get24(uint16_t i) { return ((data[i] << 16) | (data[i+1] << 8) | data[i+2]);  }
uint32_t DMX::get32(uint16_t i) { return ((data[i] << 24) | (data[i+1] << 16) | (data[i+2] << 8) | data[i+3]);  }

void DMX::update() {

    // auto frames = FPS::pool[0]->fps/std::max(44.0f,fps.fps);

    fps.run();

    for (auto r : remaps) {

        auto chan = r.first_chan;

        for (int offset = 0; offset < r.quantity; offset++) {  
            
            auto pos = (offset*r.attributes.size());

            for (int i = 0; i < r.attributes.size(); i++) { 

                float target;
                auto c = r.attributes[i].combining;
                if (c==1) target      = GMAui2f[data[chan]];
                else if (c==2) target = get16(chan)/65535.0f;
                else if (c==3) target = get24(chan)/16777215.0f;
                else if (c==4) target = get32(chan)/4294967295.0f;

                // range remap
                target = (target * (r.attributes[i].max - r.attributes[i].min)) + r.attributes[i].min;

                (*(r.dest+i+pos))= target;
                chan += c;
                
            }

        } 
    }

    for (auto cb:callbacks) cb(this); 

}