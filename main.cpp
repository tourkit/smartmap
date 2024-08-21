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
    rgb.add<float>("red").range(0,1,1.123).add<float>("green").range(0,1,2).add<float>("blue").range(0,1,3);
    Member xyz("XYZ");
    xyz.add<float>("x").range(0,1,6).add<float>("y").range(0,1,5).add<float>("z").range(0,1,4);

    Member &didoo = *new Member("Didoo");
    didoo.add<float>("didi").range(2,222,22).add<float>("dodo").range(8,888,88).striding(true);

    Member sa("Sa");
    sa.add(&rgb);

    Member sb("Sb");
    sb.add(&xyz);
    
    testbuf.add(&didoo);

    Instance didoo_(testbuf);
    didoo_.loc(&didoo);

    testbuf.add(&sa);

    // auto sa_ = Instance(testbuf)[&sa];

    // testbuf.add(&sb);

    // Instance sb_blue_(testbuf);
    // sb_blue_.loc(&sb);
    // sb_blue_.loc("XYZ");
    // sb_blue_.loc(2);
    
    // sa.quantity(12);

    // auto sa1_green_ = Instance("testbuf::Sa[9]::RGB::green");
    
    // sa.quantity(10); // nogood
    
    Instance(testbuf).print(true);

    logger.cout(Sev::verbose);
    delete &didoo;

    logger.cout(Sev::warning);
    // for (int i = 0; i < sa.quantity(); i++) 
    //     sa_.eq(i).set<float,3>((i?i:-1)*1.0f,(i?i:-1)*2.0f,(i?i:-1)*3.0f);


    // Instance i(testbuf);
    // i.loc(&sa,1);

    // i.set<float>(69);
    
    // sa1_green_.set<float>(28);

    // didoo.quantity(2);

    // sb_blue_.set<float>(987);
    // int q = testbuf.size()/4;
    Instance(testbuf).print();

}

