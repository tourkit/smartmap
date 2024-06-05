/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "engine.hpp"

int main() {

    engine.init();

    engine.open("project.json");

    engine.run();

}

// fine feedback

// glsl rotate

// artnet output

// later gater :

// deletion is fucked

// static Effector should  update on file update

// c# librehardwaremonitor set fanspeed@burne





// struct Dynamic_ubo { int a,b,c,d; };

// layout (binding = 0, std140) uniform dynamic_ubo_  { Dynamic_ubo dynamic_ubo[2]; };

// Dynamic_ubo dynubofx() { return dynamic_ubo[1]; } // is this copied or ref/ptr ?

// Dynamic_ubo dynubovar = dynamic_ubo[1]; // is this copied or ref/ptr ?

// for (int i = 0; i < 1000; i++) { dynamic_ubo[1].a } // the bracket operator happens everytime ? if so can I do better ?
