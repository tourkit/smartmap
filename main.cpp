/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

int main() { 
    
    Fixture basic =  {{2},{2},{2},{2},{2,-1,1},{2,-1,1},{2},{2},{2},{2},{2},{2},{2,0,360*(M_PI / 180.0)},{2}};

    sm.createFixtures(1,0,0,&basic);

    sm.render();
 
}
