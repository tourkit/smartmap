#include "artnet.hpp"  

#include "smartmap.hpp"

Artnet::Artnet(const char* ip) {

artnet = artnet_new(ip, 0);
artnet_set_short_name(artnet, "SmartMap");
artnet_set_long_name(artnet, "SmartMap");
artnet_start(artnet);
artnet_set_dmx_handler(artnet, [](artnet_node n, artnet_packet p, void *_this){
    for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) ((Artnet*)_this)->universes[p->data.admx.universe].raw[i] = p->data.admx.data[i]; 
    return 1; 
}, this);

}

Artnet::~Artnet() { artnet_stop(artnet); artnet_destroy(artnet); }

void Artnet::run() { artnet_read(artnet, .1); for (auto u:universes)  u.second.callback(&u.second); }

Artnet::Universe::Universe() { memset(&raw[0],0,512); steps.resize(4); for (int i = 0; i < 4; i++) { steps[i] = .000; } } 

uint16_t Artnet::Universe::get16(uint16_t i) { return ((raw[i] << 8) | raw[i+1]);  }
uint32_t Artnet::Universe::get24(uint16_t i) { return ((raw[i] << 16) | (raw[i+1] << 8) | raw[i+2]);  }
uint32_t Artnet::Universe::get32(uint16_t i) { return ((raw[i] << 24) | (raw[i+1] << 16) | (raw[i+2] << 8) | raw[i+3]);  }

void Artnet::Universe::update() { 
    for (int i = 0; i < steps.size(); i++) *(output+i) += steps[i]; 
    }

void Artnet::Universe::remap(float* output, std::vector<Attribute> attributes) {

    this->output = output;
    steps.resize(attributes.size());

    uint16_t chan = 0;
    uint16_t id = 0;
    for (int i = 0; i < attributes.size(); i++) { 

        auto c = attributes[i].combining;
        float val,target;

        if (c==1) target      = GMAui2f[raw[chan]];
        else if (c==2) target = get16(chan)/65535.0f;
        else if (c==3) target = get24(chan)/16777215.0f;
        else if (c==4) target = get32(chan)/4294967295.0f;

        // range remap
        target = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;

        val = target-*(output+id);
        val /= sm.window->fps.fps/44; // DMX_FREQUENCY should be realtime
        steps[i] = val;

        id += std::min((uint8_t)1,c); // or remapNoZero(std::vector<T>* output, std::vector<Attribute> attributes) 
        chan += std::max((uint8_t)1,c); 

    } 

    // for (int i = 0; i < 20; i++) { std::cout << steps[i] << " "; }
    // std::cout << "" << std::endl;

}



