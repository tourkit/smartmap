/*

    SMARTMAP (beta)

                        */

#include "drawcall.hpp"

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    auto l1 = engine.stack->addOwnr<Layer>();

    // auto q1 = l1->add(engine.models->child("quad"));

    // q1->add( engine.effectors->child("argb") );


    engine.gui->editors.push_back(new EditorWidget());
    engine.gui->editors.back()->locked = true;

    // engine.stack->select();
    l1->select();
    engine.gui->editors.push_back(new EditorWidget());

    engine.run();

}


// remap
// default
//range
// etc


// check for refs in remaping


//q>1 is shit.
