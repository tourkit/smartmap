/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"


int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");

    if (engine.stack->childrens.size()) engine.stack->childrens[0]->select();

    engine.run();

}
