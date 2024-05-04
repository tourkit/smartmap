/*

    SMARTMAP (beta)

                        */

#include "drawcall.hpp"

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    auto l1 = engine.stack->addOwnr<Layer>();

    auto q1 = l1->add(engine.models->child("quad"));

    q1->add( engine.effectors->child("argb") );

    PLOGD << l1->get()->s.print_recurse();

    engine.run();

}


// remap
// default
//range
// etc


// check for refs in remaping
