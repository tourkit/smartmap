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

    auto r = engine.remaps->active(true)->addOwnr<Remap>(
        &engine.inputs->childrens[0]->is_a<Artnet>()->universes[0]->data[0],
        &engine.dynamic_ubo.data[0],
        &engine.dynamic_ubo
    );

    r->select()->active(true)->onrun([](Node* n) {

         n->is_a<Remap>()->update();


    })->onchange([](Node* n) { PLOGD <<"LLOOOOOOOOOO"; });

    r->get()->attr({{0}, {1}, {0}});

    // engine.dynamic_ubo.referings.insert( engine.remaps->childrens[0] );


    engine.run();
}


// TODO next:
// - remap
// - Node movement


// Update Remap::attributes on Remap::s change without changing Member or Struct ....

// this should happen on Node update otherwise if its code just bea good dev u dum fuk

// figure out where I had similar issue and whart dumb solution I camup wiz . answer : pre_() post_() dumfuk. that bad ?
