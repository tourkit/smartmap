/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "src/engine.hpp"

#include "src/effector.hpp"

// maybe I need Instance::Link to tell Instance(StrobeFX::static_s) when Instance(StrobeFX::s) has changed

// check how Member::update() works 

// could I use referings ?

// link are between instances !

// Instance(ubersmart1::smartlayer1::argb::red).remap(ubersmart1::smartlayer1::argb::blue)
// thus both red and blue are tracked

// Instance(ubersmart1::smartlayer1::argb)["red"].set<float>(9);

struct StrobeEffector : Effector {

    int step = 2; 

    Member* strobe_s;
    
    ~StrobeEffector() { delete strobe_s; }

    StrobeEffector() {

        strobe_s = engine.glsl_data.add<int>("strobe").members.back();

    }

    bool setup(Builder* builder) override;
    
    bool body(Builder* builder, std::string prepend = "") override;


};

#include "src/instance.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");

    engine.run();

}
