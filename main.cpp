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

    ubl.addLayer(1920,1080);
    ubl.addLayer(1920,1080);
    ubl.addLayer(1920,1080);
    ubl.addLayer(1920,1080);
    ubl.addLayer(1920,1080);
    ubl.addLayer(1920,1080);


    ubl.calc_matrice(nullptr);

    engine.tree->addPtr<UberLayer>(&ubl)->active(true);

    engine.open("project.json");

    auto &l1 = *ubl.layers.begin();

    l1.addEffector(engine.effectors->childrens[0]->is_a<File>());

    UberLayer::ShaderProgramBuilder builder;
    ubl.shader.builder = &builder;
    ubl.shader.create();

    // engine.stack->childrens[0]->is_a<Layer>()->shader.create();

    // engine.tree->each([](Node* n) {

    //     ShaderProgram* shader = nullptr;
    //     if (n->is_a_nowarning<Layer>()) shader = &n->is_a<Layer>()->shader;
    //     else if (n->is_a_nowarning<UberLayer>()) shader = &n->is_a<UberLayer>()->shader;
    //     else if (n->is_a_nowarning<ShaderProgram>()) shader = n->is_a<ShaderProgram>();
    //     else return;

    //     if (shader) shader->create();

    // });

    engine.run();

}


// qui declenche layer::shader::update() on layer hierarchy change ?


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
