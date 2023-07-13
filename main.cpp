/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

SmartMap& sm = SmartMap::getInstance();

int main() { 

    // Boilerplate();
    
    SmartMap::Fixture basic = {
        {1},{1},{1},{1},    // DIM RGB
        {2,-1,1},{2,-1,1},  // POSITION
        {2},{2},            // FOCUS
        {1},{1},{1},{1},    // GOBO
        {1,0,360*(M_PI / 180.0)},{1},{1}, // BEAM : Orientation, Strobe, Feedback
        {0} // pr buffer alignment vec4 dans glsl
    }; 

    SmartMap::Layer(0, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 1, 1);
    SmartMap::Layer(19, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 1, 1);

    sm.render();
 
}





