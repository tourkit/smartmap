/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "model.hpp"
#include "artnet.hpp"
#include "drawcall.hpp"

#include "ndi.hpp"

#include "buffer.hpp"
#include "struct.hpp"

int main() {

    engine.init();

    logger.cout();

    engine.open("project.json");

    engine.run();

}



// multi layer
// ~save remap

// linuxify

// fine

// static Effector should  update on file update

// RE QUANTITY not atlas work duh

// timelines (thus player) maybeee


// node::each<T>
