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

    // engine.inputs->childrens[0]->is_a<Artnet>()->universes[0] = new DMX{0};
    // auto r = engine.remaps->active(true)->addOwnr<DMX::Remap>(
    //   &engine.inputs->childrens[0]->is_a<Artnet>()->universes[0]->data[0],
    //   &engine.dynamic_ubo.data[0],
    //   &engine.stack->childrens[0]->childrens[0]->is_a<Model>()->s
    // );
    // r->select()->active(true);
    // engine.stack->childrens[0]->childrens[0]->referings.insert(r->node());
    // r->get()->attr({{1}, {1}, {1}, {1}, {2}, {2}, {2}, {2}, {1}});

    // auto layer = engine.stack->childrens[0]->is_a<Layer>();
    // auto &ndi_sender = *engine.outputs->addOwnr<NDI::Sender>(layer->fb.width, layer->fb.height)->get();
    // engine.stack->childrens[0]->onrun([&](Node* n) {

    //   n->is_a<Layer>()->fb.read(&ndi_sender.data[0], GL_BGRA);
    //   ndi_sender.send();

    // });

    engine.run();

}

// ~save remap

// timeline

// static Effector should  update on file update

// RE QUANTITY not atlas work duh
