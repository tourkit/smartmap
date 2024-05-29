/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "layer.hpp"
#include "utils.hpp"

struct TBuilder : Layer::ShaderProgramBuilder {

    void frag() override {

        Layer::ShaderProgramBuilder::frag();

        body_fragment += "\tCOLOR = vec4(1);\n";

    }

};

int main() {

    engine.init();

    // logger.cout();

    UberLayer ubl;

    ubl.addLayer(192,108);
    ubl.addLayer(192,108);
    ubl.addLayer(192,108);

    ubl.calc_matrice(nullptr);

    engine.tree->addPtr<UberLayer>(&ubl)->active(true);

    engine.open("project.json");

    PLOGI << Layer::ShaderProgramBuilder::layout(&engine.dynamic_ubo);

    ubl.shader.create();

    auto &vbo =engine.stack->childrens[0]->is_a<Layer>()->vbo;

    // VBO vbo;
    // vbo.add(&VBO::quad);
    vbo.addQuad(.5,.5,0,0);
    vbo.upload();
    engine.tree->addPtr<VBO>(&vbo);

    engine.run();

}


// vbo not binded or some shit

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
