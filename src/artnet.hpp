#ifndef ARTNETDEVICE_H
#define ARTNETDEVICE_H

#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include "../../include/vendor/ofxLibArtnet/artnet/artnet.h"

#include "dmx.hpp"

struct Artnet {

  artnet_node artnet;

  std::map<uint16_t, DMX> universes;

  std::vector<uint16_t> listening = {0};

  std::function<void(Artnet*)> callback = [](Artnet *_this){ };

  Artnet(const char* ip);

  ~Artnet();

  void run();

} ;

#endif