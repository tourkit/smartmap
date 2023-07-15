#include "artnet.hpp"  
#include <cmath>

Artnet::Artnet(const char* ip) {

    artnet = artnet_new(ip, 0);
    artnet_set_short_name(artnet, "SmartMap");
    artnet_set_long_name(artnet, "SmartMap");
    artnet_start(artnet);
    artnet_set_dmx_handler(artnet, [](artnet_node n, artnet_packet p, void *_this){

        auto *u = &((Artnet*)_this)->universes[p->data.admx.universe];

        for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) u->raw[i] = p->data.admx.data[i]; 

        auto frames = FPS::pool[0]->fps/std::max(44.0f,u->fps.fps);
        u->fps.name = ("Universe "+std::to_string(p->data.admx.universe)).c_str();
        u->fps.run();

        for (auto cb:u->callbacks) cb(u);  

        if (((Artnet*)_this)->listening.size()) if (p->data.admx.universe == ((Artnet*)_this)->listening.back()) ((Artnet*)_this)->callback((Artnet*)_this);

        return 1;   

    }, this);

}

Artnet::~Artnet() { artnet_stop(artnet); artnet_destroy(artnet); }

void Artnet::run() { artnet_read(artnet, .1); }

Artnet::Universe::Universe() { memset(&raw[0],0,512); } 

uint16_t Artnet::Universe::get16(uint16_t i) { return ((raw[i] << 8) | raw[i+1]);  }
uint32_t Artnet::Universe::get24(uint16_t i) { return ((raw[i] << 16) | (raw[i+1] << 8) | raw[i+2]);  }
uint32_t Artnet::Universe::get32(uint16_t i) { return ((raw[i] << 24) | (raw[i+1] << 16) | (raw[i+2] << 8) | raw[i+3]);  }


void Artnet::Universe::remap(uint16_t chan, uint16_t quantity, std::vector<Attribute>& attributes, float* dest) {

    for (int offset = 0; offset < quantity; offset++) {  
        
        auto pos = (offset*attributes.size());

        for (int i = 0; i < attributes.size(); i++) { 

            float target;
            auto c = attributes[i].combining;
            if (c==1) target      = GMAui2f[raw[chan]];
            else if (c==2) target = get16(chan)/65535.0f;
            else if (c==3) target = get24(chan)/16777215.0f;
            else if (c==4) target = get32(chan)/4294967295.0f;

            // range remap
            target = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;

            (*(dest+i+pos))= target;
            chan += c;
            
        }

    } 

}

