/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"
int WIDTH = 600, HEIGHT = 300;
auto* window = new GL::Window(false,WIDTH,HEIGHT);
 
#include <vector>
#include <iostream>
 
#include "ofxLibArtnet/artnet/artnet.h"

int main() {

  std::vector<uint8_t> data = {127,255};

  artnet_node artnet = artnet_new("2.0.0.222", 1); // set ip la !

  artnet_set_short_name(artnet, "SmartMap");
  artnet_set_long_name(artnet, "SM");
  artnet_set_node_type(artnet, ARTNET_RAW); // ARTNET_SRV not working either

  artnet_set_subnet_addr(artnet, 0); //

  artnet_set_port_type(artnet, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(artnet, 0, ARTNET_OUTPUT_PORT, 1); // port 0 , link address 1

  artnet_dump_config(artnet);

  if (artnet_start(artnet) != 0) std::cout << artnet_strerror() << std::endl;

  std::cout << artnet_send_dmx(artnet, 0, data.size(), &data[0]) <<std::endl; // envoi a univers 0 ... au lieu de 1 (port 0)
  std::cout << artnet_raw_send_dmx(artnet, 1, data.size(), &data[0]) <<std::endl; // en voi a univers 3 ...
  std::cout << artnet_raw_send_dmx(artnet, 2, data.size(), &data[0]) <<std::endl;  // n'envoi rien


  // output : only two univers : 0 & 3 ......

  artnet_stop(artnet);
  artnet_destroy(artnet);

  return 0;

} 

