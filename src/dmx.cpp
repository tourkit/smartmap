#include "dmx.hpp"
#include "struct.hpp"
#include "log.hpp"
#include "struct.hpp"
#include "artnet.hpp"

DMX::DMX(int id) : id(id) { memset(&data[0],0,512); PLOGD << "universe " << id;  } 


DMX::Fixture::Fixture(Struct* s) : s(s) {

    // for (auto& m : s->members) c->each([this](Member &m){ attributes.push_back({1,m.range_from,m.range_to}); });

}

void DMX::update() {

    for (auto r : remaps) r.update();
    
    for (auto cb:callbacks) cb(this); 

}

DMX::Remap::Remap(void *src, void *dst, Fixture *f, uint16_t q) { 

    this->src = (char*)src;
    this->dest = (char*)dst;
    this->fixture = f;
    this->quantity = q;

    attributes = fixture->attributes;
    
} 


uint8_t DMX::Remap::get8(uint8_t* data) { return GMAui2f[*data]; }

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
            if (c) (*(((float*)dest)+i+pos)) = (target * (fixture->attributes[i].max - fixture->attributes[i].min)) + fixture->attributes[i].min;
            else c = 1;
            
            data += c;
            
        }

    } 

}

