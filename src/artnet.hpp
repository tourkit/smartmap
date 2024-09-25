#ifndef ARTNETDEVICE_H
#define ARTNETDEVICE_H

#include <vector>
#include <string>
#include <map>
#include "vendors/ofxLibArtnet/artnet/artnet.h"

#include "member.hpp"
#include "remap.hpp"

struct DMXRemap : Remap {

    struct Attribute { int combining; float min=0, max=1; bool active = true;};// !combining is JUMP member

    int chan;

    std::vector<Attribute> attributes;

    int quantity;

    DMXRemap(Instance *src, Instance *dst, int chan = 0, std::vector<Attribute> attributes = {}, int quantity = 1); // Maybe chan could be in inst already thoo
    ~DMXRemap(){

PLOGV << "catch";

    }

    void update() override;

    void extract(Member *s);

};

struct Artnet;
struct Universe : Member{ 
   Artnet* an; int id; Instance* instance = nullptr; ~Universe(); Universe(Artnet* an, int id); 
};
struct Artnet : Member{


  static inline std::vector<std::string> available_ips;

  artnet_node artnet = NULL;


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
