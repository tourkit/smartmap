/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

int main() {  

    sm.createFixtures(1);

    sm.artnet->universes[0].callback = [&]() { sm.artnet->universes[0].remap<float>(&sm.fixtureUBO->data, {{2},{2},{2},{2},{2},{2},{2},{2},{2},{2},{2},{2},{2,0,3.6},{2}}); sm.fixtureUBO->update(); };

    sm.render();
 
}