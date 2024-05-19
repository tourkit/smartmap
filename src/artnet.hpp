#ifndef ARTNETDEVICE_H
#define ARTNETDEVICE_H

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <array>
#include <functional>
#include <iostream>
#include "../../vendors/ofxLibArtnet/artnet/artnet.h"

#include "dmx.hpp"
#include "buffer.hpp"
#include "struct.hpp"
#include "instance.hpp"

struct DMXRemap : Remap {

    struct Attribute { int combining; float min=0, max=1; bool active = true;};// !combining is JUMP member

    int chan;

    std::vector<Attribute> attributes;

    int quantity;

    DMXRemap(Instance*src, Instance*dst, int chan = 0, std::vector<Attribute> attributes = {}, int quantity = 1); // Maybe chan could be in inst already thoo

    void update() override;

    void extract(Member *s);

};

struct Artnet : Buffer {

  static inline Struct &uni_s = Struct::create("universe").add<char>("chan",512);

  artnet_node artnet = NULL;

  struct Universe : Struct { Artnet* an; int id;  Universe(Artnet* an, int id); };

  std::map<int, std::shared_ptr<Universe>> universes;

  std::vector<uint16_t> listening = {0};

  Artnet(std::string ip = "");

  ~Artnet();

  void run();

  void connect(std::string ip);

  Universe& universe(int id);

  void disconnect();

  std::string ip;

  int device_id = 0;

} ;

#endif
