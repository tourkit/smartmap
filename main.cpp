/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */



#include "src/member.hpp"
#include "src/instance.hpp"
#include "src/log.hpp"





int main() {

    logger.cout(Sev::verbose);

    Member testbuf("testbuf");
    testbuf.buffering(true);

    Member rgb("RGB");

    rgb.add<float>("red").range(0,1,1).add<float>("green").range(0,1,2).add<float>("blue").range(0,1,3);

    Member didoo("didoo");
    didoo.add<float>("didi").add<float>("dodo").striding(true);
    
    Member sa("Sa");
    sa.add(&rgb);

    Member sb("Sb");
    sb.add(&rgb);

    testbuf.add(&didoo);

    testbuf.add(&sa);
    
    testbuf.add(&sb);

    auto sb_ = Instance("testbuf::Sb::RGB::green").track();

    PLOGD << sb_.offset;

    sa.quantity(10);

    PLOGD << sb_.offset;

    Instance testbuf_("testbuf::Sa[1]::RGB::red");
    
    testbuf_.track();

    Instance testbuf_2(testbuf);
    testbuf_2.loc(&sa,1);
    testbuf_2.loc("RGB");
    testbuf_2.loc(0);

    PLOGD << testbuf_.stl.back().m->name() << " " << testbuf_.offset;
    PLOGD << testbuf_2.stl.back().m->name() << " " << testbuf_.offset;;

    


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

