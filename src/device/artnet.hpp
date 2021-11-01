#ifndef ARTNETDEVICE_H
#define ARTNETDEVICE_H

#include <vector>
#include <map>

#include "../attribute.hpp"
#include "../../include/vendor/ofxLibArtnet/artnet/artnet.h"


struct DMXAttribute : public Attribute {

	int address;

	DMXAttribute(int address) : address(address), Attribute(std::to_string(address).c_str(), 0) {  }

	virtual void update(uint8_t* data) { set(data[address]/255.); }

  void set(float val) override {  for (auto l:links_dst) l->setNormalized(val); }

};

struct DMXAttributeFine : public DMXAttribute {

  DMXAttributeFine( int address) : DMXAttribute(address) {  }

  void update(uint8_t* data) override { set(__builtin_bswap16((uint16_t&)data[address])/65535.); }

};

struct DMXUniverse {

  uint8_t chan[512];

  std::vector<DMXAttribute*> links;

  void update() { for (auto l:links) l->set(chan[l->address]); }

  ~DMXUniverse() {for(auto l:links) delete l;}

};

struct ArtnetDevice {

  artnet_node artnet;

  static inline std::map<uint16_t,DMXUniverse> data;

  ArtnetDevice() {
    
    artnet = artnet_new("2.0.0.222", 0); // set ip la !
    artnet_set_short_name(artnet, "SmartMap");
    artnet_set_long_name(artnet, "SmartMap");
    artnet_start(artnet);
    artnet_set_dmx_handler(artnet, dmx_callback, NULL);
  
  }

  static int dmx_callback(artnet_node n, artnet_packet p, void *d) {

    auto& dmx = p->data.admx;

    data[dmx.universe]; // ca coute qq chose ca a chaque tcheck une fois qu'il est deja créé ?

    for(int i = 0; i < __builtin_bswap16((uint16_t&)dmx.lengthHi); ++i) data[dmx.universe].chan[i] = dmx.data[i];

    for (auto l : data[dmx.universe].links) l->update(&data[dmx.universe].chan[0]); // ou data[dmx.universe].update(); mais pas cool avec 16bit

    return 0;
    
  }

  ~ArtnetDevice() { 

    artnet_stop(artnet);
    artnet_destroy(artnet);

  }
  
  void run() {

    artnet_read(artnet, .1);  

  }

} ;

#endif