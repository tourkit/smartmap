/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "ndi.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
#include "widgets/fps.hpp"

#include "shader_builder.hpp"
#include "model.hpp"
#include "test.hpp"

// shader sources
// clocks
// fix feedback blending



int main() { 

    Log log;

    Test test;
    
    auto &engine = Engine::getInstance();
    engine.window.setPos(2560,0);
    engine.init();

    // SmartMap::Base sm;

    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();

    // sm.config.import("config.json");

    engine.run();
 
}


