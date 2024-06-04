/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "layer.hpp"
#include "utils.hpp"
#include "artnet.hpp"
#include "texture.hpp"


int main() {

    engine.init();

    engine.open("project.json");

    // ubl_->referings.insert(engine.inputs->childrens[0]->childrens[1]);

    engine.run();

}


// fine feedback

// glsl rotate

// artnet output

// later gater :

// deletion is fucked

// static Effector should  update on file update

// c# librehardwaremonitor set fanspeed@burne
