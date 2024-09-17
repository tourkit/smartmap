

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

//engine own effector in hidden

// refaire marcher buikder

//  voir comment DC gere lplusieurs models 



int main() {




    // auto xxx = &f3( std::forward<Args>(args)... );//*(&f<Node>);

    // xxx("123");

    // arbitrary, but convertible types (C++14)

    // Boilerplate::Init();

    logger.cout(Sev::warning);

    engine.init();

    // logger.cout(Sev::verbose);
    engine.open("project.json");
    // logger.cout(Sev::verbose);



    // auto lay_ = engine.stack->addOwnr<Layer>(400,300);
    // auto ubl_ = engine.stack->addOwnr<UberLayer>();
    // auto &ubl = *ubl_->is_a<UberLayer>();
    // auto vl = ubl.addLayer(1920, 1080);
    // vl.quantity(2);
    // ubl.calc_matrice();
    // auto vl_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl);




    engine.run();



 

}
