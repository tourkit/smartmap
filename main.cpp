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

    auto &uni0 = engine.inputs->childrens[0]->is_a<Artnet>()->uni(0).instances[0];

    uni0.remaps.push_back( new DMXRemap(&uni0, &engine.dynamic_ubo["layer1"]["MyQuad"]["argb"].track(), 0, {{1},{1},{1},{1}}) );

    PLOGD << engine.dynamic_ubo["layer1"]["MyQuad"]["argb"].offset;

    engine.run();

}


// ~save remap

// multi layer

// linuxify

// fine

// static Effector should  update on file update

// RE QUANTITY not atlas work duh

// timelines (thus player) maybeee


// node::each<T>
