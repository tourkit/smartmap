/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */



#include "engine.hpp"
#include "model.hpp"

int main() {

    // logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");  

    logger.cout(Sev::verbose);
    delete engine.tree->find("Free Layer 1"); // Member on(delete, {for remaps ðelte remap})
    logger.cout(Sev::warning);

    engine.run();

}
