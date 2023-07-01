#include "artnet.hpp"  

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