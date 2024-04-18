/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "remap.hpp"
#include "artnet.hpp"

int main() {

    engine.init();

    engine.open("project2.json");

    logger.cout();

    // PLOGD << engine.inputs->childrens[0]->type().name();
 engine.inputs->childrens[0]->is_a<Artnet>()->universes[0] = new DMX{0};

 engine.window.keypress_cbs[GLFW_KEY_F] = [](int key) { PLOGD <<  engine.inputs->childrens[0]->is_a<Artnet>()->universes[0]->data[0]; };

    engine.remaps->active(true)->addOwnr<Remap>(
        &engine.inputs->childrens[0]->is_a<Artnet>()->universes[0]->data[0],
        &engine.dynamic_ubo.data[0],
        &engine.dynamic_ubo
    )->select()->active(true)->onrun([](Node* n) {

         n->is_a<Remap>()->update();


    })->onchange([](Node* n) { PLOGD <<"LLOOOOOOOOOO"; });

    // engine.dynamic_ubo.referings.insert( engine.remaps->childrens[0] );


    engine.run();
}


// TODO next:
// - remap
// - Node movement


// Update Remap::attributes on Remap::s change without changing Member or Struct ....
