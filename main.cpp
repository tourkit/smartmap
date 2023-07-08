/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

SmartMap& sm = SmartMap::getInstance();

int main() { 

    // Boilerplate();
    
    Fixture basic =  {{1},{1},{1},{1},{2,-1,1},{2,-1,1},{2},{2},{1},{1},{1},{1},{1,0,360*(M_PI / 180.0)},{1}};

    sm.createFixtures(1,0,0,&basic);

    sm.render();
 
}
