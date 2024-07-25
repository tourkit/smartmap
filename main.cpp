/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");

    auto &stack = *engine.stack;
    auto s2 = stack[2];
    auto s0 = stack[0];
    s2->add(s0);

    engine.run();

}
