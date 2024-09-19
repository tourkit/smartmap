

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


#include "atlas.hpp"
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
#include "utils.hpp"
#include <string>
#include <vector>

// fix Member::adding all occurence set default  ..

//  voir comment DC gere lplusieurs models 

// fix right menu

// fix open shit


int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");

    engine.window.size(100, 100);

    auto quad_ = engine.tree->find("quad");
    auto main = engine.tree->find("main");
    auto argb = engine.tree->find("argb");
    auto rectangle = engine.tree->find("rectangle");


    main->add(quad_);
    auto quad= main->childrens.back()->childrens.back();
    quad->add(argb);
    main->add(quad_);
    auto quad_1 = main->childrens.back()->childrens.back();
    quad_1->name("quad_1");
    quad_1->add(argb);
    quad_1->add(rectangle);
    logger.cout(Sev::verbose);
    quad->add(rectangle);

    // NEED Node::layer_1->udate()

    // delete quad;
    logger.cout(Sev::warning);

    engine.gui_v->selected = main->childrens.back();

    engine.run();



}
