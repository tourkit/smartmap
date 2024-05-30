/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "layer.hpp"
#include "utils.hpp"
#include "artnet.hpp"


int main() {

    engine.init();

    UberLayer ubl;

    ubl.addLayer(1920,1080).s.quantity(5);

    ubl.calc_matrice(nullptr);

    engine.tree->addPtr<UberLayer>(&ubl)->active(true);

    auto &l1 = *ubl.layers.begin();
    engine.dynamic_ubo.add(&l1.s);

    engine.open("project.json");

    l1 = *ubl.layers.begin();

    l1.addEffector(engine.effectors->childrens[0]->is_a<File>());
    l1.addEffector(engine.effectors->childrens[3]->is_a<File>());
    l1.addEffector(engine.effectors->childrens[2]->is_a<File>());
    engine.stack->trigchange();


    engine.run();

}

// engine.tree->addOwnr<Node>("dsjkhf")->onchange([](Node* n){ PLOGD << "NONOO"; }); // Uberlayer not working with this ... maybe ptr loss somewhere ?
// auto n = engine.inputs->childrens[0]->addPtr<DMXRemap>(dmxremap)->name("booyaremap"); // same with this

// UberLayer

// linuxify a.k.a gl1.1 is the pb

// artnet output

// fine feedback

// later gater :

// ~save remap

// static Effector should  update on file update

// sort && thus add fx & models accordingly

// timelines (thus player) maybeee

// model editor about instance :) c ( engine.selected should be instance? inst is only members.)

// drop remap
