/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "effector.hpp"
#include "src/engine.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");

    auto x = engine.stack->childrens[1]->childrens[0]->childrens[0]->is_a<EffectorRef>();

    PLOGW << x->s.size();

    engine.run();

}
