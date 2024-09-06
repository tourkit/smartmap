/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

// ----

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("project.json");

    engine.run();

}
