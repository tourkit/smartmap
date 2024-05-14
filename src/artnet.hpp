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

  artnet_node artnet = NULL;

  std::map<uint16_t, DMX *> universes;

  std::vector<uint16_t> listening = {0};

  std::function<void(Artnet*)> callback = [](Artnet *_this){ };

  Artnet(std::string ip = nullptr);

  ~Artnet();

  void run();

  void connect(std::string ip);
  void disconnect();

  std::string ip;

  int device_id = 0;

  DMX * uni(int id);

} ;

#endif
