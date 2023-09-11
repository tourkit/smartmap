/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"

// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

#include "buffer.hpp"

int main() { 

    // Boilerplate();

    auto &engine = Engine::getInstance();

    SmartMap sm;

    SmartMap::Layer l1(0, 0, sm.basic_fixture, engine.window.width,engine.window.height, SmartMap::Layer::Mode::Free, 1,1);

    // SmartMap::Layer l2(80, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,1);
    
    // SmartMap::Layer l2(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,2);
    
    Engine::Run();
 
}




    

