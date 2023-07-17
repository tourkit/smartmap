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
        {1,0,360*(M_PI / 180.0)},{1},{1}, // BEAM : Orientation, Strobe, Feedback
        {0} // pr buffer alignment vec4 dans glsl
    }; 

    std::vector<float> a1 = {
        1,0,1,1,   
        .3,.3, 
        .2,.2,           
        0,0,0,0, 
        0,0,0, 
        0 
    }; 
    std::vector<float> a2 = {
        1,1,0,1,   
        0.6,0.6, 
        .2,.2,           
        0,0,0,0, 
        0,0,0, 
        0 
    }; 

    sm.fixtureUBO->update(&a1[0],a1.size()*4);
    sm.fixtureUBO2->update(&a2[0],a2.size()*4);

    SmartMap::Layer l1(0, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 2, 1);
    SmartMap::Layer l2(38, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 1, 1);
    
    // SmartMap::Layer l2(0, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 3,2);

    for (int i = 0; i < 9; i++) { 

        auto j = i*4;

        for (int k = 0; k < 4; k++) { std::cout << sm.matriceUBO->data[j+k] << " ";  }

        std::cout << "" << std::endl;


     }

    sm.render();
 
}





