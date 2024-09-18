

/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype

// grid / quantity 

// ----


#include "boilerplate.hpp"
#include "drawcall.hpp"
#include "editor.hpp"
#include "engine.hpp"
#include "builder.hpp"
#include "imgui.h"
#include "layer.hpp"
#include "gui.hpp"
#include "member.hpp"
#include "node.hpp"
#include <string>
#include <vector>

// fix Member::adding all occurence set default  ..

//  voir comment DC gere lplusieurs models 


int main() {

    // Boilerplate::Init();

    logger.cout(Sev::warning);

    engine.init();


    // logger.cout(Sev::verbose);
    engine.open("project.json");


    logger.cout(Sev::warning);

    // test hard_delete childrens, does ~Node ?
    // on some layer I create

    // auto lay_ = engine.stack->addOwnr<Layer>(400,300);
    // auto ubl_ = engine.stack->addOwnr<UberLayer>();
    // auto &ubl = *ubl_->is_a<UberLayer>();
    // auto vl = ubl.addLayer(1920, 1080);
    // vl.quantity(2);
    // ubl.calc_matrice();
    // auto vl_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl);

    engine.run();



}
