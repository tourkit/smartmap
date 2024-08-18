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

    Buffer testbuf("testbuf");

    Struct rgb("RGB");
    rgb.add<float>("red").add<float>("green").add<float>("blue");

    Struct sa("Sa");
    sa.add(&rgb);
    testbuf.add(&sa);

    Struct sb("Sb");
    sb.add(&rgb);
    testbuf.add(&sb);

    Instance inst(&testbuf);

    inst.each([&](Instance& inst) {

        std::string lst;
        for (auto x : inst.stl) lst += x->name() + ", ";
        if (lst.size()) lst = "[" + lst.substr(0, lst.size() - 2) + "]";

        PLOGW << inst.def()->name() << " - " << inst.offset << " - " << lst;

    });

    inst = inst.find(&sb)[0][0].track();

    PLOGW << inst.def()->name();

    std::string stl_name = "Sb::RGB::red";

    Instance inst2(stl_name);

    // inst.find("Sb::RGB::red");



    engine.open("./project.json");



    engine.run();

}
