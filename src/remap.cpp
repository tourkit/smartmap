#include "remap.hpp"

#include "struct.hpp"
#include "struct.hpp"

uint8_t Remap::get8(uint8_t* data) { return data[0]; }
uint16_t Remap::get16(uint8_t* data) { return ((data[0] << 8) | data[1]);  }
uint32_t Remap::get24(uint8_t* data) { return ((data[0] << 16) | (data[1] << 8) | data[2]);  }
uint32_t Remap::get32(uint8_t* data) { return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);  }

void Remap::import(Struct *s) {

    // this->s = s;

    // for (auto c: s->comps) {

    //     for (auto &m: c->members) {

    //         attributes.push_back({1,m.range_from,m.range_to});
            
    //     }

    // }

}



Remap::Remap() {}

Remap::Remap(void *src, void *dst, Struct *s, uint16_t q) 
: src((char*)src), dest((char*)dst), s(s), quantity(q) { 

    import(s);
    update(); 
    
}


void Remap::update() {
    
    auto data = (uint8_t*)src;

    for (int offset = 0; offset < quantity; offset++) {  

        auto size = s->size();
        auto pos = (offset*size);

        for (int i = 0; i < attributes.size(); i++) {

            float target = 0;

            auto c = attributes[i].combining;

            if (c==1) target      = get8(data);
            else if (c==2) target = get16(data)/65535.0f;
            else if (c==3) target = get24(data)/16777215.0f;
            else if (c==4) target = get32(data)/4294967295.0f;

            // range remap
            if (c) (*(((float*)dest)+i+pos)) = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;
            else c = 1;
            
            data += c;
            
        };

    } 

}

