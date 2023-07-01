/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

int main() {  

    Fixture basic =  {{2},{2},{2},{2},{2},{2},{2},{2},{2},{2},{2},{2},{2,0,3.6},{2}};

    sm.createFixtures(1,0,0,&basic);

    sm.render();
 
}