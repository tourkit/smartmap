/*

    SMARTMAP v1 beta

                        */

// #include "smartmap.hpp"
// int WIDTH = 600, HEIGHT = 300;
// auto* window = new GL::Window(false,WIDTH,HEIGHT);
 
#include <iostream>

#include "device/artnet.hpp"
#include "fixture.hpp"

ArtnetDevice artnet;

int main() {

  std::vector<std::string> fixture_chart = {

    "Color.Red",
    "Position.Horizontal --FINE",
    "Color.Blue",
    "Color.Green --FINE"

  };

  Fixture* fifix1 = new Fixture();
  fifix1->DMXpatch(2,1,fixture_chart);

  Fixture* fifix2 = new Fixture();
  fifix2->DMXpatch(1,14,fixture_chart);

  while(true) {

    artnet.run();

  }

  return 0;

} 

