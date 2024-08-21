/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */



#include "src/member.hpp"
#include "src/instance.hpp"
#include "src/log.hpp"


int main() {

    logger.cout(Sev::warning);

    Member testbuf("testbuf");
    testbuf.buffering(true);

    Member rgb("RGB");

    rgb.add<float>("red").range(0,1,1).add<float>("green").range(0,1,2).add<float>("blue").range(0,1,3);

    Member didoo("didoo");
    didoo.add<float>("didi").range(6,666,66).add<float>("dodo").range(9,999,99).striding(true);

    Member sa("Sa");
    sa.add(&rgb);

    Member sb("Sb");
    sb.add(&rgb);
    
    testbuf.add(&didoo);

    testbuf.add(&sa);

    // auto sa_ = Instance(testbuf)[&sa];
    Instance sa_(testbuf);
    sa_.loc(&sa);
    // exit(0);

    testbuf.add(&sb);
    // auto blue_ = Instance("testbuf::Sb::RGB::blue");
    Instance blue_("testbuf::Sb::RGB::blue");

    sa.quantity(10);

    didoo.quantity(2);

    for (int i = 0; i < sa.quantity(); i++) 
        sa_.eq(i).set<float,3>((i?i:-1)*1.0f,(i?i:-1)*2.0f,(i?i:-1)*3.0f);

    auto i = Instance(testbuf);
    i.loc(&sa,1);
    i.set<float>(987);

    blue_.set<float>(69);

    PLOGW << Instance(testbuf).get<float,41>();

    logger.cout(Sev::verbose);

    logger.cout(Sev::error);

}

