/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "test.hpp"

int main() { 
    
    auto &engine = Engine::getInstance();

    engine.init();

    Test test;

    engine.run();
 
}


