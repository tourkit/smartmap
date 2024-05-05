/*

    SMARTMAP (beta)

                        */

#include "drawcall.hpp"

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    // auto l1 = engine.stack->addOwnr<Layer>();

    // // auto q1 = l1->add(engine.models->child("quad"));

    // // q1->add( engine.effectors->child("argb") );


    // engine.gui->editors.push_back(new EditorWidget());
    // engine.gui->editors.back()->locked = true;

    // // engine.stack->select();
    // l1->select();
    // engine.gui->editors.push_back(new EditorWidget());
    engine.open("project2.json");

    auto &x = Effector::get(engine.effectors->child("argb")->is_a<File>());
    PLOGD << x.s.name();


    // engine.run();

}


// default

// no instasnce at open
