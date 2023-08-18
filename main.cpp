/*

    SMARTMAP (beta)

                        */

// #include "smartmap.hpp"
#include "engine.hpp"
#include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;



int main() { 

    // Boilerplate();


    // SmartMap& sm = SmartMap::getInstance();

    // SmartMap::Layer l1(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 4,1);

    // SmartMap::Layer l2(80, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,1);
    
    // SmartMap::Layer l2(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,2);


    auto &engine = Engine::getInstance();

    UBO::fromJSON();
    

    Engine::Run();


    // while(true) window->render([&]() {

    // }

    // sm.render();
 



}




    

