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

    Component::init();

    for (auto c : Component::pool) {
        
        std::cout << c->name << std::endl;
        for (auto m : c->members) std::cout << " - " << m.name << " (" << m.range_from << "," << m.range_to << ")" << std::endl;
        
    }
    
    SmartMap::Layer l1(0, 0, sm.basic_dmxremap, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 5, 5);

    // SmartMap::Layer l2(0, 1, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 2,2);
    
    // SmartMap::Layer l2(0, 0, basic, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,2);

    sm.render();
 



}






