#include "remap.hpp"

#include "struct.hpp"
#include "member.hpp"

float Remap::get8(uint8_t* data) { return data[0]/255.0f; }
uint16_t Remap::get16(uint8_t* data) { return ((data[0] << 8) | data[1]);  }
uint32_t Remap::get24(uint8_t* data) { return ((data[0] << 16) | (data[1] << 8) | data[2]);  }
uint32_t Remap::get32(uint8_t* data) { return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);  }

void Remap::extract(Member *s) {

    for (auto m : s->members) {

        if (m->isData()) {


            auto cc = m->count();

            for (int i = 0 ; i < cc; i++)
            attributes.push_back({1,m->range<float>()[0],m->range<float>()[1]});

        }else{

            extract(m);

        }

    }

}



Remap::Remap() {}

Remap::Remap(void *src, void *dst, Member *s, uint16_t q)

    : src((char*)src), dest((char*)dst), s(s) {

    extract(s);

    quantity = s->quantity();


}


void Remap::reset() {

    auto bkp = attributes;

    attributes.resize(0);

    extract(s);

    quantity = s->quantity();

    for (int i = 0 ; i < (bkp.size()>attributes.size()?attributes.size():bkp.size()); i++) attributes[i] = bkp[i];

}

void Remap::attr(std::vector<Attribute> attributes) {

    if (attributes.size()>this->attributes.size()) attributes.resize(this->attributes.size());

    for (int i = 0 ; i <  attributes.size(); i++) this->attributes[i] = attributes[i];

}


void Remap::update() {

    auto data = (uint8_t*)src;

    for (int offset = 0; offset < quantity; offset++) {

        auto size = s->size();

        auto pos = (offset*size);
        pos /=sizeof(float);

        for (int i = 0; i < attributes.size(); i++) {

            float target = 0;

            auto c = attributes[i].combining;

            if (c==1) target      = get8(data);
            else if (c==2) target = get16(data)/65535.0f;
            else if (c==3) target = get24(data)/16777215.0f;
            else if (c==4) target = get32(data)/4294967295.0f;

            // range remap
            if (attributes[i].active && c > 0) *((float*)dest+i+pos) = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;

            data += c;

        };

    }

}
