/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "layer.hpp"


int main() {

    engine.init();

    UberLayer ubl;

    ubl.add(192,108);
    ubl.add(192,108);
    ubl.add(192,108);

    ubl.calc(nullptr);

    engine.tree->addPtr<UberLayer>(&ubl)->active(true);

    engine.open("project.json");

    PLOGI << Layer::ShaderProgramBuilder::layout(&engine.dynamic_ubo);

    engine.run();

}



// layout from UBO directly not layer effector list

// VBO::addquad()

// UberLayer: Drawcall

// UberLayer::Vlayer

// UberShaderProgramBuilder




// matrice pass

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
