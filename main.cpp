/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */



#include "src/member.hpp"
#include "src/instance.hpp"
#include "src/log.hpp"
#include <cstdint>





int main() {

    logger.cout(Sev::verbose);


    Member rgb("RGB");

    rgb.add<float>("red");
    
    rgb.range(0,1,1).add<float>("green").range(0,1,2).add<float>("blue").range(0,1,3);

    Member didoo("didoo");
    didoo.add<float>("didi").add<float>("dodo").striding(true);
    
    PLOGD << "-->" << didoo.footprint();

    Member sa("Sa");
    sa.add(&rgb);

    Member sb("Sb");
    sb.add(&rgb);
    
    Member testbuf("testbuf");
    testbuf.buffering(true);

    testbuf.add(&didoo);

    testbuf.add(&sa);
    
    testbuf.add(&sb);

    auto sb_ = Instance("testbuf::Sb::RGB::blue").track();


    Instance testbuf_("testbuf::Sa[1]::RGB::red");
    
    // testbuf_.track();

    Instance testbuf_2(testbuf);
    testbuf_2.loc(&sa,1);
    testbuf_2.loc("RGB");
    testbuf_2.loc(0);

    // for (auto x : rgb.getTop())  PLOGD << x->name();
    PLOGD << testbuf_.stl.back().m->name() << " " << testbuf_.offset;
    PLOGD << testbuf_2.stl.back().m->name() << " " << testbuf_.offset;;
    
    
    PLOGD << "-------------";


    PLOGD << sb_.offset;

    sa.quantity(10);

    PLOGD << sb_.offset;
    
    PLOGD << "-------------";
    
    Instance(testbuf).each([&](Instance& inst) {

        PLOGD << inst.stl_name() << "[" << inst.stl.back().m->footprint_all()<< "]" << " " << inst.offset;

    });

    // auto &sa_ = testbuf.add(&sa).track();
    
    // Member sx("Sx");
    // sx.add(&didoo);
    // auto &sx_ = testbuf.add(&sx).track();
    
    // Member sb("Sb");
    // sb.add(&rgb);
    // auto &sb_ = testbuf.add(&sb).track();

    // sb_.set<float,3>(6.0f,66.0f,666.0f);

    // sx.quantity(2);

    // sa.quantity(10);
    
    // Instance("testbuf::Sb::RGB::red").remap("testbuf::Sa[2]::RGB::green");

    // for (int i = 0; i < 10; i++) 
    //     sa_.eq(i).set<float,3>(i*1.0f,i*2.0f,i*3.0f);

    // PLOGW << Instance(&testbuf).get<float,41>();

    // sb_[0][0].set<float>(987);

    // PLOGW << Instance(&testbuf).get<float,41>();

    // Instance(&testbuf).print();


    logger.cout(Sev::error);

}

