/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boi/lerplate.hpp"
// #include "debug.hpp"
// Debug debug;

SmartMap& sm = SmartMap::getInstance();

int main() { 

    // Boilerplate();
    
    SmartMap::Fixture basic = {
        {1},{1},{1},{1},    // DIM RGB
        {2},{2},  // POSITION
        {2},{2},            // FOCUS
        {1},{1},{1},{1},    // GOBO
        {1},{1},{1}, // BEAM : Orientation, Strobe, Feedback
        {0} // pr buffer alignment vec4 dans glsl
    }; 

    // SmartMap::Layer l1(0, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 2, 1);
    // SmartMap::Layer l2(38, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 2, 1);
    
    SmartMap::Layer l2(0, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 1,1);

    sm.render();
 
}





