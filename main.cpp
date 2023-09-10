/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"

#include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

#include "buffer.hpp"

int main() { 

    // Boilerplate();

    // SmartMap::Layer l1(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 4,1);

    // SmartMap::Layer l2(80, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,1);
    
    // SmartMap::Layer l2(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,2);

    auto &engine = Engine::getInstance();

    SmartMap sm;

    engine.stack.list.push_back(new Engine::Stack::DrawCall{&sm.quad, &sm.shader});

    Engine::Run();
 
}




    

