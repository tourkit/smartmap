#ifndef ARTNETDEVICE_H
#define ARTNETDEVICE_H

#include <vector>
#include <map>
#include <functional>
#include <iostream>

#include "../../include/vendor/ofxLibArtnet/artnet/artnet.h"

static inline float GMAui2f[256] = {0, 0.00390625, 0.0078125, 0.0117188, 0.015625, 0.0195312, 0.0234375, 0.0273438, 0.03125, 0.0351562, 0.0390625, 0.0429688, 0.046875, 0.0507812, 0.0546875, 0.0585938, 0.0625, 0.0664062, 0.0703125, 0.0742188, 0.078125, 0.0820312, 0.0859375, 0.0898438, 0.09375, 0.0976562, 0.101562, 0.105469, 0.109375, 0.113281, 0.117188, 0.121094, 0.125, 0.128906, 0.132812, 0.136719, 0.140625, 0.144531, 0.148438, 0.152344, 0.15625, 0.160156, 0.164062, 0.167969, 0.171875, 0.175781, 0.179688, 0.183594, 0.1875, 0.191406, 0.195312, 0.199219, 0.203125, 0.207031, 0.210938, 0.214844, 0.21875, 0.222656, 0.226562, 0.230469, 0.234375, 0.238281, 0.242188, 0.246094, 0.25, 0.253906, 0.257812, 0.261719, 0.265625, 0.269531, 0.273438, 0.277344, 0.28125, 0.285156, 0.289062, 0.292969, 0.296875, 0.300781, 0.304688, 0.308594, 0.3125, 0.316406, 0.320312, 0.324219, 0.328125, 0.332031, 0.335938, 0.339844, 0.34375, 0.347656, 0.351562, 0.355469, 0.359375, 0.363281, 0.367188, 0.371094, 0.375, 0.378906, 0.382812, 0.386719, 0.390625, 0.394531, 0.398438, 0.402344, 0.40625, 0.410156, 0.414062, 0.417969, 0.421875, 0.425781, 0.429688, 0.433594, 0.4375, 0.441406, 0.445312, 0.449219, 0.453125, 0.457031, 0.460938, 0.464844, 0.46875, 0.472656, 0.476562, 0.480469, 0.484375, 0.488281, 0.492188, 0.496094, 0.5, 0.503968, 0.507937, 0.511905, 0.515873, 0.519841, 0.52381, 0.527778, 0.531746, 0.535714, 0.539683, 0.543651, 0.547619, 0.551587, 0.555556, 0.559524, 0.563492, 0.56746, 0.571429, 0.575397, 0.579365, 0.583333, 0.587302, 0.59127, 0.595238, 0.599206, 0.603175, 0.607143, 0.611111, 0.615079, 0.619048, 0.623016, 0.626984, 0.630952, 0.634921, 0.638889, 0.642857, 0.646825, 0.650794, 0.654762, 0.65873, 0.662698, 0.666667, 0.670635, 0.674603, 0.678571, 0.68254, 0.686508, 0.690476, 0.694444, 0.698413, 0.702381, 0.706349, 0.710317, 0.714286, 0.718254, 0.722222, 0.72619, 0.730159, 0.734127, 0.738095, 0.742063, 0.746032, 0.75, 0.753906, 0.757812, 0.761719, 0.765625, 0.769531, 0.773438, 0.777344, 0.78125, 0.785156, 0.789062, 0.792969, 0.796875, 0.800781, 0.804688, 0.808594, 0.8125, 0.816406, 0.820312, 0.824219, 0.828125, 0.832031, 0.835938, 0.839844, 0.84375, 0.847656, 0.851562, 0.855469, 0.859375, 0.863281, 0.867188, 0.871094, 0.875, 0.878906, 0.882812, 0.886719, 0.890625, 0.894531, 0.898438, 0.902344, 0.90625, 0.910156, 0.914062, 0.917969, 0.921875, 0.925781, 0.929688, 0.933594, 0.9375, 0.941406, 0.945312, 0.949219, 0.953125, 0.957031, 0.960938, 0.964844, 0.96875, 0.972656, 0.976562, 0.980469, 0.984375, 0.988281, 0.992188, 0.996094, 1};

struct Artnet {

  struct Universe { 
      
    uint8_t raw[512]; 
    Universe() { memset(&raw[0],0,512); } 

    std::function<void(Universe*)> callback = [](Universe* uni){};

    uint16_t get16(uint16_t i) { return ((raw[i] << 8) | raw[i+1]);  }
    uint32_t get24(uint16_t i) { return ((raw[i] << 16) | (raw[i+1] << 8) | raw[i+2]);  }
    uint32_t get32(uint16_t i) { return ((raw[i] << 24) | (raw[i+1] << 16) | (raw[i+2] << 8) | raw[i+3]);  }

    template<typename T>
    struct Attribute { uint8_t combining; T min=0,max=1; };

    template<typename T>
    std::vector<T> remap(std::vector<Attribute<T>> attributes) { std::vector<T> remapped; remapped.resize(attributes.size()); remap(&remapped, attributes); return remapped; }

    template<typename T>
    void remap(std::vector<T>* output, std::vector<Attribute<T>> attributes) {

        uint16_t chan = 0;
        uint16_t id = 0;
        for (int i = 0; i < attributes.size(); i++) { 

            auto c = attributes[i].combining;

            if (c==1) (*output)[id] = (raw[chan]/255.0f);
            else if (c==2) (*output)[id] = (get16(chan)/65535.0f);
            else if (c==3) (*output)[id] = (get24(chan)/16777215.0f);
            else if (c==4) (*output)[id] = (get32(chan)/4294967295.0f);

            (*output)[id] = ((*output)[id] * (attributes[i].max - attributes[i].min)) + attributes[i].min;

            id += std::min((uint8_t)1,c); // or remapNoZero(std::vector<T>* output, std::vector<Attribute> attributes) 
            chan += std::max((uint8_t)1,c); 

        } 

    }
    

  };

  artnet_node artnet;

  std::map<uint16_t, Universe> universes;

  Artnet(const char* ip) {
    
    artnet = artnet_new(ip, 0);
    artnet_set_short_name(artnet, "SmartMap");
    artnet_set_long_name(artnet, "SmartMap");
    artnet_start(artnet);
    artnet_set_dmx_handler(artnet, [](artnet_node n, artnet_packet p, void *_this){
      for(int i = 0; i < __builtin_bswap16((uint16_t&)p->data.admx.lengthHi); ++i) ((Artnet*)_this)->universes[p->data.admx.universe].raw[i] = p->data.admx.data[i]; 
      return 1; 
    }, this);
  
  }

  ~Artnet() { artnet_stop(artnet); artnet_destroy(artnet); }
  
  void run() { artnet_read(artnet, .1); for (auto u:universes)  u.second.callback(&u.second); }

} ;

#endif