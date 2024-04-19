/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "remap.hpp"
#include "artnet.hpp"
#include "model.hpp"

int main() {

    engine.init();

    engine.open("project2.json");

    logger.cout();

   engine.inputs->childrens[0]->is_a<Artnet>()->universes[0] = new DMX{0};

    auto r = engine.remaps->active(true)->addOwnr<DMX::Remap>(
        &engine.inputs->childrens[0]->is_a<Artnet>()->universes[0]->data[0],
        &engine.dynamic_ubo.data[0],
        engine.stack->childrens[0]->childrens[0]->is_a<Model>()
    );
    r->select()->active(true);

    r->get()->attr({{1}, {1}, {1}, {1}, {2}, {2}, {1}});
    r->get()->quantity = 3;

    engine.stack->childrens[0]->childrens[0]->referings.insert(r->node());
    // engine.dynamic_ubo.referings.insert( engine.remaps->childrens[0] );


    engine.run();
}


// TODO next:
// - Node movement
