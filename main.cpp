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

    logger.cout(Sev::verbose);

    Buffer testbuf("testbuf");

    Struct rgb("RGB");
    rgb.add<float>("red").add<float>("green").add<float>("blue");
    Struct didoo("didoo");
    didoo.add<float>("dada").striding(true);
    
    Struct sa("Sa");
    sa.add(&rgb);
    auto sa_ = testbuf.add(&sa).track();
    sa_.set<float,3>(1.0f, 2.0f, 3.0f);

    Struct sb("Sb");
    sb.add(&rgb);
    auto sb_ = testbuf.add(&sb);
    sb_.set<float,3>(4.0f, 5.0f, 6.0f);

    auto &didoo_ = testbuf.add(&didoo).track();
    didoo_.set<float>(69);

    auto &green_ = Instance(&testbuf).find(&sb)[0][1].track();

    PLOGW << sa_.get<float, 3>();
    PLOGW << green_.offset;

    delete rgb.members[2];

    testbuf.remove(sa);

    auto &green2_ = Instance(&testbuf).find(&sb)[0][1].track();
    PLOGW << green2_.offset << " " << green_.def()->name() << " "  << green_.get<float>();

    PLOGW << Instance(&testbuf).find(&sb).offset << " " ;
    PLOGW << didoo_.offset << " " ;
    didoo_.set<float>(96);
    PLOGW << *(std::array<float,3>*)testbuf.data.data();

    
    logger.cout(Sev::error);
    exit(0);

    // check if remap is consistent when already is a tracked

    Instance(nullptr,&sa).each([&](Instance& inst) {

        std::string lst;
        for (auto x : inst.stl) lst += x->name() + ", ";
        if (lst.size()) lst = "[" + lst.substr(0, lst.size() - 2) + "]";

        PLOGW << inst.def()->name() << " - " << inst.offset << " - " << lst;

    });


    PLOGW << green_.def()->name();

    // std::string stl_name = "Sb::RGB::red";

    // Instance green2_(stl_name);

    // inst.find("Sb::RGB::red");



    engine.open("./project.json");



    engine.run();

}
