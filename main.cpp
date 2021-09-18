/*

    SMARTMAP v1 beta

                        */

// #include "smartmap.hpp"
// int WIDTH = 600, HEIGHT = 300;
// auto* window = new GL::Window(false,WIDTH,HEIGHT);
 
#include <vector>
#include <iostream>
 
#include "ofxLibArtnet/artnet/artnet.h"

int8_t buff[ARTNET_DMX_LENGTH +1];
int length;

int dmx_callback(artnet_node n, int port, void *d) {
  std::cout << "OOOO" <<std::endl;
  uint8_t *data;

  data = artnet_read_dmx(n, port, &length);
  memset(buff, 0x00, ARTNET_DMX_LENGTH+1);
  memcpy(&buff[1], data, length);

  printf("got dmx on port %i\n", artnet_get_universe_addr(n, port, ARTNET_OUTPUT_PORT) );

  return 0;
}


int main() {

  std::vector<uint8_t> data = {127,255};

  artnet_node artnet = artnet_new("2.0.0.222", 1); // set ip la !

  artnet_set_short_name(artnet, "SmartMap");
  artnet_set_long_name(artnet, "SM");
  // artnet_set_node_type(artnet, ARTNET_NODE); // ARTNET_SRV not working either
  artnet_set_bcast_limit(artnet, 0);
  artnet_set_subnet_addr(artnet, 0); //

  artnet_set_port_type(artnet, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(artnet, 0, ARTNET_OUTPUT_PORT, 3);
  // artnet_set_port_type(artnet, 0, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX);
  // artnet_set_port_addr(artnet, 0, ARTNET_INPUT_PORT, 0);

  if (artnet_start(artnet) != 0) std::cout << artnet_strerror() << std::endl;
  artnet_send_poll(artnet, NULL, ARTNET_TTM_DEFAULT);

  artnet_set_dmx_handler(artnet,dmx_callback,NULL);

  
  // while(true) for (uint8_t i = 0; i < 10; i++) artnet_raw_send_dmx(artnet, i, data.size(), &data[0]);
  while(true) artnet_send_dmx(artnet, 0, data.size(), &data[0]);
  // while(true) artnet_read(artnet, 0);

  artnet_stop(artnet);
  artnet_destroy(artnet);

  return 0;

} 

