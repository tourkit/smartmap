

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

// cant delete model with 2 effectors



int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");



    auto quad = engine.tree->find("quad");
    auto main = engine.tree->find("main");
    auto argb = engine.tree->find("argb");
    auto rectangle = engine.tree->find("rectangle");
    main->add(quad);
    auto model = main->childrens.back()->childrens.front();
    model->add(argb);
    model->add(rectangle);


    logger.cout(Sev::verbose);

    delete main->childrens.back()->childrens.front();

    for (auto x : engine.window.end_of_render_cbs) 
        x.second(x.first);

    engine.window.end_of_render_cbs.clear();

    logger.cout(Sev::warning);

    engine.run();



}
