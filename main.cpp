/*

    SMARTMAP v1 beta

                        */

// #include "smartmap.hpp"
// int WIDTH = 600, HEIGHT = 300;
// auto* window = new GL::Window(false,WIDTH,HEIGHT);
 
#include <vector>
#include <iostream>
 
#include "ofxLibArtnet/artnet/artnet.h"

int main() {

  artnet_node artnet = artnet_new("2.0.0.222", 0); // set ip la !
  artnet_set_short_name(artnet, "SmartMap");
  artnet_set_long_name(artnet, "SmartMap");
  
  artnet_start(artnet);

  

   while(true) {

    // std::cout << artnet_read(artnet, .1) << std::endl;

  }


  artnet_stop(artnet);
  artnet_destroy(artnet);

  return 0;

} 

