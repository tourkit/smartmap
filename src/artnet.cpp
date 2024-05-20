#include "artnet.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "../../vendors/ofxLibArtnet/artnet/misc.h"
#include <cmath>

void DMXRemap::update() {

  auto data = (uint8_t*)src->data()+chan;

    for (int offset = 0; offset < dst->def()->quantity(); offset++) {

        auto size = dst->def()->size();

        auto pos = (offset*size);
        pos /=sizeof(float);

        for (int i = 0; i < attributes.size(); i++) {

            float target = 0;

            auto c = attributes[i].combining;

            if (c==1) target      = data[0]/255.0;
            else if (c==2) target = ((data[0] << 8) | data[1])/65535.0f;
            else if (c==3) target = ((data[0] << 16) | (data[1] << 8) | data[2])/16777215.0f;
            else if (c==4) target = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3])/4294967295.0f;

            // range remap
            if (attributes[i].active && c > 0) *((float*)dst->data()+i+pos) = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;

            data += c;

        };

    }

}

void DMXRemap::extract(Member *s) {

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

DMXRemap::DMXRemap(Instance*src, Instance*dst, int chan, std::vector<DMXRemap::Attribute> attrs, int quantity)

    : Remap(src, dst), chan(chan), attributes(attrs),quantity(quantity) {

    if (!attributes.size()) extract(dst->def());

}


Artnet::Artnet(std::string ip)  { connect(ip); }

  Artnet::Universe& Artnet::universe(int id) {

    try { return *universes.at(id).get(); }

    catch(const std::out_of_range& e) {

        uint32_t offset = universes.size()*512;

        auto x = universes.emplace(id, std::make_shared<Artnet::Universe>(this, id)).first->second.get();

        add(x);

        Instance{this,offset,{x}}.track();

        PLOGD << id;

        return *x;

    }

  }


Artnet::Universe::Universe(Artnet* an, int id) : Struct(""+std::to_string(id)), an(an), id(id) {

    add(&uni_s);

}


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

        auto &u = an->universe(uni_id).instances[0];

        u.set<std::array<char,512>>(p->data.admx.data);

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

    universes.clear();
}

void Artnet::run() {
    if (artnet) {
        if (artnet_read(artnet, .1))
            PLOGE << "artnet_read ERROR: " << artnet_errstr;
    }
}
