/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project.json");

    engine.run();
}



// movement

//remap
