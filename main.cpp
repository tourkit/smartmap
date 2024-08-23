/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "GL/gl3w.h"
#include "callbacks.hpp"
#include "editors.hpp"
#include "engine.hpp"

// #include "src/effector.hpp"

// maybe I need Member::Link to tell StrobeFX::static_s when StrobeFX::s has changed

// check how Member::update() works 

// could I use referings ?

// struct StrobeEffector : Effector {

//     int step = 2; 

//     Member* strobe_s;
    
//     ~StrobeEffector() { delete strobe_s; }

//     StrobeEffector() {

//         strobe_s = engine.glsl_data.add<int>("strobe").members.back();

//     }

//     bool setup(Builder* builder) override;
    
//     bool body(Builder* builder, std::string prepend = "") override;


// };


int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("project.json");

    engine.run();

}
