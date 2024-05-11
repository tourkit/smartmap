#ifndef ARTNETDEVICE_H
#define ARTNETDEVICE_H

#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include "../../vendors/ofxLibArtnet/artnet/artnet.h"

#include "dmx.hpp"

// template <int I>
// struct Universe : DMX { Universe() : DMX() { id = I; } };

struct Artnet {

  artnet_node artnet;

  std::map<uint16_t, DMX *> universes;

  std::vector<uint16_t> listening = {0};

  std::function<void(Artnet*)> callback = [](Artnet *_this){ };

  Artnet(std::string ip = nullptr);

  ~Artnet();

  void run();

  // ArtnetWindow gui;

} ;

#endif
