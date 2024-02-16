/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
// #include "boilerplate.hpp"

#include "engine.hpp"
#include "test.hpp"

int main() { 



    // Boilerplate();
    
    auto &engine = Engine::getInstance();

    engine.init();

    Test test;

    engine.run();
 
}


