#include "dmx.hpp"
#include "struct.hpp"

DMX::DMX() { memset(&data[0],0,512); } 

uint16_t DMX::get16(uint16_t i) { return ((data[i] << 8) | data[i+1]);  }
uint32_t DMX::get24(uint16_t i) { return ((data[i] << 16) | (data[i+1] << 8) | data[i+2]);  }
uint32_t DMX::get32(uint16_t i) { return ((data[i] << 24) | (data[i+1] << 16) | (data[i+2] << 8) | data[i+3]);  }



DMX::Fixture::Fixture(Struct* s) { 

    for (auto c : s->comps) {

        for (auto m: c->members){
            
            uint8_t q = 1;
            if (m.type == Component::Member::Type::VEC2) q =2;
            if (m.type == Component::Member::Type::VEC3) q =3;
            if (m.type == Component::Member::Type::VEC4) q =4;
            for (int i = 0; i < q; i++) attributes.push_back({1,m.range_from,m.range_to});

        }

    }


}

void DMX::update() {

    for (auto r : remaps) {

        auto chan = r.first_chan;

        for (int offset = 0; offset < r.quantity; offset++) {  
            
            auto pos = (offset*r.fixture->attributes.size());

            for (int i = 0; i < r.fixture->attributes.size(); i++) { 

                float target;
                auto c = r.fixture->attributes[i].combining;
                if (c==1) target      = GMAui2f[data[chan]];
                else if (c==2) target = get16(chan)/65535.0f;
                else if (c==3) target = get24(chan)/16777215.0f;
                else if (c==4) target = get32(chan)/4294967295.0f;

                // range remap
                target = (target * (r.fixture->attributes[i].max - r.fixture->attributes[i].min)) + r.fixture->attributes[i].min;

                (*(((float*)r.dest)+i+pos)) = target;
                chan += c;
                
            }

        } 
    }
    
    for (auto cb:callbacks) cb(this); 

}