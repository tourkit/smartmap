/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "test.hpp"
#include "file.hpp"

int main() { 
    
    auto &engine = Engine::getInstance();

    engine.init();

    Test test;

    engine.run();
 
}


