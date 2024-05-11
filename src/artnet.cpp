#include "artnet.hpp"
#include "log.hpp"
#include "../../vendors/ofxLibArtnet/artnet/misc.h"
#include <cmath>

extern "C" {

iface_t* artnet_list_ifaces();
iface_t* artnet_list_ifaces_next(iface_t* ift);
void artnet_free_ifaces(iface_t* ift);

}

Artnet::Artnet(std::string ip) {

    std::string using_ip  = ip;

    if (!ip.length()) {

        auto y= artnet_list_ifaces();

        for (auto ift = y; ift != NULL; ift = artnet_list_ifaces_next(ift)) {

            ip = inet_ntoa(ift->ip_addr.sin_addr);

            PLOGD << "available NIC : " << ip;

            if ( ip[0] == 50 || ( ip[0] == 49 && ip[1] == 48 ) ) using_ip = ip;

        }

        if (!using_ip.length()) { PLOGW << "NO NETWORK INTERRFACE FOUND"; return; }

         PLOGW << "using ip : " << using_ip;

    }

    artnet = artnet_new(using_ip.c_str(), 0); // 1 for VERBOSE
    if (!artnet) {
        PLOGE << "artnet_new ERROR: " << artnet_errstr;

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
