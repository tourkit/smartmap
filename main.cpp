/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "test.hpp"
#include "log.hpp"

int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();

    engine.init();

    Test test;

    engine.run();
 
}


