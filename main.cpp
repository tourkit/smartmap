/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "src/engine.hpp"
#include "src/layer.hpp"
#include "src/shader.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");



    engine.run();

}
