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

#include "src/effector.hpp"

// maybe I need Instance::Link to tell Instance(StrobeFX::static_s) when Instance(StrobeFX::s) has changed

// check how Member::update() works 

// could I use referings ?

// link are between instances !

// Instance(ubersmart1::smartlayer1::argb::red).remap(ubersmart1::smartlayer1::argb::blue)
// thus both red and blue are tracked

// Instance(ubersmart1::smartlayer1::argb)["red"].set<float>(9);

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

    // track could have owner list

    // inst.print()

    engine.init();

    logger.cout(Sev::warning);

    Buffer testbuf("testbuf");

    Struct rgb("RGB");
    rgb.add<float>("red").range(0,1,1).add<float>("green").range(0,1,2).add<float>("blue").range(0,1,3);

    Struct didoo("didoo");
    didoo.add<float>("didi").range(8,8,8).add<float>("dodo").range(9,9,9).striding(true);
    
    Struct sa("Sa");
    sa.add(&rgb);
    auto &sa_ = testbuf.add(&sa).track();
    
    Struct sx("Sx");
    sx.add(&didoo);
    auto &sx_ = testbuf.add(&sx).track();
    
    Struct sb("Sb");
    sb.add(&rgb);
    auto &sb_ = testbuf.add(&sb).track();

    sb_.set<float,3>(6.0f,66.0f,666.0f);

    sx.quantity(2);

    sa.quantity(10);
    
    Instance("testbuf::Sb::RGB::red").remap("testbuf::Sa[2]::RGB::green");

    for (int i = 0; i < 10; i++) 
        sa_.eq(i).set<float,3>(i*1.0f,i*2.0f,i*3.0f);

    PLOGW << Instance(&testbuf).get<float,41>();

    sb_[0][0].set<float>(987);

    PLOGW << Instance(&testbuf).get<float,41>();

    Instance(&testbuf).print();

    logger.cout(Sev::error);

    exit(0);

    engine.open("./project.json");

    engine.run();

}
