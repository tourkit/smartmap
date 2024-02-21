#include "dmx.hpp"
#include "struct.hpp"
#include "log.hpp"
#include "component.hpp"
#include "artnet.hpp"

DMX::DMX(int id) : id(id) { memset(&data[0],0,512); PLOGV << "universe " << id;  } 


DMX::Fixture::Fixture(Struct* s) : s(s) {

    for (auto c : s->comps) c->each([this](Member &m){ attributes.push_back({1,m.range_from,m.range_to}); });

}

void DMX::update() {

    for (auto r : remaps) r.update();
    
    for (auto cb:callbacks) cb(this); 

}

uint16_t DMX::Remap::get16(uint8_t* data) { return ((data[0] << 8) | data[1]);  }
uint32_t DMX::Remap::get24(uint8_t* data) { return ((data[0] << 16) | (data[1] << 8) | data[2]);  }
uint32_t DMX::Remap::get32(uint8_t* data) { return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);  }

void DMX::Remap::update() {
    
    auto data = (uint8_t*)src;

    for (int offset = 0; offset < quantity; offset++) {  
        
        auto pos = (offset*fixture->attributes.size());

        for (int i = 0; i < fixture->attributes.size(); i++) { 

            float target = 0;
            auto c = fixture->attributes[i].combining;
            if (c==1) target      = GMAui2f[*data];
            else if (c==2) target = get16(data)/65535.0f;
            else if (c==3) target = get24(data)/16777215.0f;
            else if (c==4) target = get32(data)/4294967295.0f;

            // range remap
            target = (target * (fixture->attributes[i].max - fixture->attributes[i].min)) + fixture->attributes[i].min;

            (*(((float*)dest)+i+pos)) = target;

            data += c;
            
        }

    } 

}

