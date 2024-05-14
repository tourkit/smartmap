/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "model.hpp"
#include "artnet.hpp"
#include "drawcall.hpp"

#include "ndi.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project.json");

    engine.run();

}

// ~save remap

// multi layer

// linuxify

// fine

// static Effector should  update on file update

// RE QUANTITY not atlas work duh

// timelines maybeee
