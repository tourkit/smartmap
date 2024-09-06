/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype


// ----


#include "engine.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("project.json");

    engine.run();

}
