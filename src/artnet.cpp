#include "artnet.hpp"
#include "log.hpp"
#include "../../vendors/ofxLibArtnet/artnet/misc.h"
#include <cmath>

Artnet::Artnet(const char* ip) {

    artnet = artnet_new(ip, 0); // 1 for VERBOSE
    if (!artnet) {
        PLOGE << "artnet_new ERROR: " << artnet_errstr;

        // artnet_list_ifaces();

artnet_new(ip, 1);

        return;
    }

    // artnet_
    artnet_set_short_name(artnet, "SmartMap");
    artnet_set_long_name(artnet, "SmartMap");
    if (artnet_start(artnet)) {
        PLOGE << "artnet_start ERROR: " << artnet_errstr;
        return;
    }
    artnet_set_dmx_handler(artnet, [](artnet_node n, artnet_packet p, void *_this) {

        auto *an = (Artnet*)_this;

        if (an->universes.find(p->data.admx.universe) == an->universes.end()) an->universes[p->data.admx.universe] = new DMX{p->data.admx.universe};

        auto u = an->universes[p->data.admx.universe];

        for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) u->data[i] = p->data.admx.data[i];

        u->update();

        if (an->listening.size()) if (p->data.admx.universe == an->listening.back()) an->callback(an);

        return 1;

    }, this);

}

Artnet::~Artnet() {
    if (artnet) {
        if (artnet_stop(artnet))
            PLOGE << "artnet_stop ERROR: " << artnet_errstr;
        artnet_destroy(artnet);
    }
}

void Artnet::run() {
    if (artnet) {
        if (artnet_read(artnet, .1))
            PLOGE << "artnet_read ERROR: " << artnet_errstr;
    }
}
