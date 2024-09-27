/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


#include "engine.hpp"
#include "layer.hpp"

int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");  

    // engine.tree->find("main")->allow<UberLayer>();
    // engine.tree->find("main")->addOwnr<UberLayer>();

    engine.run();

}
