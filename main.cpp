/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


#include "engine.hpp"

int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");

    engine.run();

}
