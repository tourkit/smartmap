/*

    SMARTMAP (beta)

                        */

// #include "smartmap.hpp"
#include "engine.hpp"
#include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

#include "buffer.hpp"

int main() { 

    // Boilerplate();

    // SmartMap& sm = SmartMap::getInstance();

    // SmartMap::Layer l1(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Grid, 4,1);

    // SmartMap::Layer l2(80, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,1);
    
    // SmartMap::Layer l2(0, 0, sm.basic_fixture, sm.window->width,sm.window->height, SmartMap::Layer::Mode::Free, 4,2);

    auto &engine = Engine::getInstance();

    auto *shader = new ShaderProgram({"basic.frag", "basic.vert"});

    shader->use();

    // engine.stack.list.push_back(Engine::Stack::DrawCall{engine.quad, shader});


    Engine::Run();
 
}




    

