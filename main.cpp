/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "src/engine.hpp"
#include "instance.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");


    Instance inst(&engine.dynamic_ubo);
    
    auto eq1 = inst.eq(1);
    PLOGW << inst.eq(1).offset;

    engine.run();

}
