/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "remap.hpp"

int main() {

    engine.init();

    // logger.cout();

    engine.open("project2.json");

    engine.remaps->addOwnr<Remap>();


    engine.run();
}


// TODO next:
// - remap // who create Remap in engine.remaps ?
// - Node movement
