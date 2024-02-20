#include "artnet.hpp"
#include "../../include/vendor/ofxLibArtnet/artnet/misc.h"
#include <cmath>

Artnet::Artnet(const char* ip) 
// : gui(this) 
{

    artnet = artnet_new(ip, 1); // 1 for VERBOSE
    if (!artnet) {
        PLOGW << "artnet_new ERROR: " << artnet_errstr;
        return;
    }

    // artnet_
    artnet_set_short_name(artnet, "SmartMap");
    artnet_set_long_name(artnet, "SmartMap");
    if (artnet_start(artnet)) {
        PLOGW << "artnet_start ERROR: " << artnet_errstr;
        return;
    }
    artnet_set_dmx_handler(artnet, [](artnet_node n, artnet_packet p, void *_this) {

        auto *an = (Artnet*)_this;

        // an->gui.updateList(p->data.admx.universe); 
        auto *u = &an->universes[p->data.admx.universe];


        for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) u->data[i] = p->data.admx.data[i]; 

        u->update(); 

        if (an->listening.size()) if (p->data.admx.universe == an->listening.back()) an->callback(an);

        return 1;   

    }, this);

}

Artnet::~Artnet() {
    if (artnet) {
        if (artnet_stop(artnet))
            PLOGW << "artnet_stop ERROR: " << artnet_errstr;
        artnet_destroy(artnet);
    }
}

void Artnet::run() {
    if (artnet) {
        if (artnet_read(artnet, .1))
            PLOGW << "artnet_read ERROR: " << artnet_errstr;
    }
}
