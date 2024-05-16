#include "artnet.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "../../vendors/ofxLibArtnet/artnet/misc.h"
#include <cmath>

Artnet::Artnet(std::string ip)  { connect(ip); }

  Artnet::UniStruct& Artnet::uni(int id) {

    try { return universes.at(id); }

    catch(const std::out_of_range& e) { return universes.emplace(id, Artnet::UniStruct(this, id)).first->second; }

  }


Artnet::UniStruct::UniStruct(Buffer* b, int id) : b(b), Struct("uni "+std::to_string(id), 3) { PLOGD << "new uni " << id; add(&universe); b->add(this); }

void Artnet::connect(std::string ip_) {

    disconnect();

    ip = ip_;

    std::string using_ip;

    if (!engine.available_ips.size()) { PLOGW << "NO NETWORK INTERRFACE FOUND ! "; return; }

    for (int i = 0 ; i < engine.available_ips.size(); i++) if (!strcmp( engine.available_ips[i].c_str(), ip.c_str() )) { using_ip = engine.available_ips[i]; device_id = i; break; }

    if (!using_ip.length()) {

        using_ip = engine.available_ips[0];

        for (int i = 0 ; i < engine.available_ips.size(); i++){

            auto available_ip = engine.available_ips[i];

            if ( available_ip[0] == 50 || ( available_ip[0] == 49 && available_ip[1] == 48 ) ) {using_ip = available_ip; break; } // ( find 10. or 2. ip preferably)

        }

        PLOGW <<  using_ip << " ! ";

    }

    if (ip == using_ip) PLOGI << using_ip;

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

        int uni_id = p->data.admx.universe;

        // an->uni(uni_id);

        // if (universes.find(uni) == universes.end()) universes[uni] = new Universe(uni);

        // an->universes[id]


        // auto u = an->universes[p->data.admx.universe];

        // for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) u->data[i] = p->data.admx.data[i];

        // u->update();

        // if (an->listening.size()) if (p->data.admx.universe == an->listening.back()) an->callback(an);

        return 1;

    }, this);

}

Artnet::~Artnet() { disconnect(); }

void Artnet::disconnect() {
    if (artnet != NULL) {
        if (artnet_stop(artnet))
            PLOGE << "artnet_stop ERROR: " <<  artnet_errstr;
        artnet_destroy(artnet);
    }
}

void Artnet::run() {
    if (artnet) {
        if (artnet_read(artnet, .1))
            PLOGE << "artnet_read ERROR: " << artnet_errstr;
    }
}
