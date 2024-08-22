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

    Member &diidoo = *new Member("Diidoo");
    diidoo.add<float,2>("didi").range(2,222,22).striding(true);

    Member sa("Sa");
    sa.add(&rgb);

    testbuf.add(&diidoo);

    Instance didoo_(testbuf);
    didoo_.loc(&diidoo);

    testbuf.add(&sa);

    diidoo.quantity(2);
    
    Instance(testbuf).print();

    delete &diidoo;
    
    Instance(testbuf).print();

}