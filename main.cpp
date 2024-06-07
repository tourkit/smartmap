/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "engine.hpp"
#include "layer.hpp"
#include "texture.hpp"
#include "editor.hpp"
#include "node.hpp"

struct SmartLayer : Layer {

    int draw_count = 1;

    SmartLayer() : Layer(1920,1080) {}

    void draw() override {

        if (feedback) { feedback->bind(); }

        fb.clear();

        shader.use();

        vbo.draw(draw_count);

        if (feedback) { return feedback->read(fb.texture); }

    }

};

// Editor<SmartLayer>([](Node* node, SmartLayer* layer) {


// });

// on new Node detect if is a from known list of types

// change Node::type() to *ptr

// compare typeid(n->ptr)

int main() {



    engine.init();

    engine.open("project.json");

    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);

    auto sl_ = engine.stack->addOwnr<SmartLayer>();
    auto &sl = *sl_->get();
if (dynamic_cast<Layer*>(&sl) != nullptr) std::cout  <<"OOOOOOOOOOOOOOOOO";
    if (typeid(*sl_->ptr) == typeid(Layer)) { std::cout << "SISIS" << std::endl; }


    engine.run();

}


// Layer::dyninst && Layer::statinst (also for editors ;) though need per Effectable ... oulalal)

// if layer model size == 1 use instanceID draw mujltipt @ shaderbuild

// cpu transform matrice matrice_elem_XY

// try 3d movement

// fine feedback ~ish

// freetype

// artnet output

// later gater :

// deletion is fucked

// static Effector should  update on file update

// c# librehardwaremonitor get temps &set fanspeed@burne

// npcap


// EDID SELECT HAPPEN ON RESTART ONLY

// EXTRA CONTROLER CRASH - NO TURN OFF ON RACK HARD KILL THEN WRONG PACKE T// need auto restart on crash (aka WRONG PACKET)

// SIGNALKILL/EDIDMAN NOT WORK W/OUT EXTRA CONTROLER // more linked to mainboard ?

// CODING WHEEL REALLY SHITY // NOT INTUITIVE MISLEADING , NEED 4 click button wheel OR 4 button not both

// MOVE EDID MENU PER OUTPUT IN OUTPUT->X->EDID

// DISPLAY INFO MESSAGE ON START / SHUTDOWN  "STARTING" "SHUTTING DOWN"

// NETWORK REPEAT FAILURE
