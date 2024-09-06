/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype


// ----



#include "effector.hpp"
#include "engine.hpp"
#include "layer.hpp"





int main() {

    engine.init();

    logger.cout(Sev::warning);


    engine.open("project.json");

    engine.run();

}
