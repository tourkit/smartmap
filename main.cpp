/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"

// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

int main() { 

    // Boilerplate();

    auto &engine = Engine::getInstance();

    SmartMap sm;

    sm.import("config.json");

    Engine::Run();
 
}




    

