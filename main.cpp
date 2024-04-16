/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project.json");

    engine.run();
}


// TODO next:
// - remap
// - Node movement
