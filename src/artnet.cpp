#include "artnet.hpp"  
#include <cmath>

Artnet::Artnet(const char* ip) {

    artnet = artnet_new(ip, 0);
    artnet_set_short_name(artnet, "SmartMap");
    artnet_set_long_name(artnet, "SmartMap");
    artnet_start(artnet);
    artnet_set_dmx_handler(artnet, [](artnet_node n, artnet_packet p, void *_this){

        auto *u = &((Artnet*)_this)->universes[p->data.admx.universe];

        for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) u->data[i] = p->data.admx.data[i]; 

        u->update(); 

        if (((Artnet*)_this)->listening.size()) if (p->data.admx.universe == ((Artnet*)_this)->listening.back()) ((Artnet*)_this)->callback((Artnet*)_this);

        return 1;   

    }, this);

}

Artnet::~Artnet() { artnet_stop(artnet); artnet_destroy(artnet); }

void Artnet::run() { artnet_read(artnet, .1); }


