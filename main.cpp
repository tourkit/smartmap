/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "layer.hpp"
#include "utils.hpp"


int main() {

    engine.init();

    UberLayer ubl;

    ubl.addLayer(192,108);
    ubl.addLayer(192,108);
    // ubl.addLayer(192,108);

    ubl.calc_matrice(nullptr);

    engine.tree->addPtr<UberLayer>(&ubl)->active(true);

    engine.open("project.json");

    // UberLayer::ShaderProgramBuilder builder;

    // ubl.shader.create(&builder);

    engine.run();

}



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
