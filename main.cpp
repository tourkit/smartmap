/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "model.hpp"
#include "artnet.hpp"



#include "ndi.hpp"



int main() {

    engine.init();

    logger.cout();

    engine.open("project2.json");

   engine.inputs->childrens[0]->is_a<Artnet>()->universes[0] = new DMX{0};

    auto r = engine.remaps->active(true)->addOwnr<DMX::Remap>(
        &engine.inputs->childrens[0]->is_a<Artnet>()->universes[0]->data[0],
        &engine.dynamic_ubo.data[0],
        &engine.stack->childrens[0]->childrens[0]->is_a<Model>()->s
    );
    r->select()->active(true);

    engine.stack->childrens[0]->childrens[0]->referings.insert(r->node());

    r->get()->attr({{1}, {1}, {1}, {1}, {2}, {2}, {2}, {2}, {1}});

auto layer = engine.stack->childrens[0]->is_a<Layer>();
  // NDI::Sender ndi_sender(layer->fb.width, layer->fb.height);
  //   engine.stack->childrens[0]->onrun([&](Node* n) {

  //     auto layer = n->is_a<Layer>();
  //     layer->fb.read();
  //     ndi_sender.send(layer->fb.);

  //   });


    engine.run();




}

// ~save

// static Effector should  update on file update

// RE QUANTITY not atlas work duh
