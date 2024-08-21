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

    auto sa_ = Instance(testbuf);

    sa_.loc(&sa);
    
    didoo.quantity(2);
    
    testbuf.add(&sb);

    Instance sb_(testbuf);

    sb_.loc(&sb);
    
    sa.quantity(10);


    Instance i(testbuf);
    i.loc(&sa,1);

    didoo.quantity(2);
    
    PLOGW << "=====================";

    for (int i = 0; i < sa.quantity(); i++) 
        sa_.eq(i).set<float,3>((i?i:-1)*1.0f,(i?i:-1)*2.0f,(i?i:-1)*3.0f);

    i.set<float>(987);


    // Instance(testbuf).each([]( Instance inst){PLOGW << inst.stl_name() << " " << inst.offset; });

    PLOGW << Instance(testbuf).get<float,41>();

    logger.cout(Sev::verbose);

    logger.cout(Sev::error);

}

