/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
// #include "smartmap.hpp"
#include "test.hpp"

// shader sources
// clocks
// fix feedback blending


int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();
    engine.window.setPos(2560,0);
    engine.init();

    Test test;

    engine.run();
 
}


