/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "test.hpp"
#include "log.hpp"
// #include "boilerplate.hpp"

int main() { 



    // Boilerplate();

    Log log;
    
    auto &engine = Engine::getInstance();

    engine.init();

    Test test;

    engine.run();
 
}


