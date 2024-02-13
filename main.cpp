/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "test.hpp"

int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();

    engine.init();

    Test test;

    engine.run();
 
}


