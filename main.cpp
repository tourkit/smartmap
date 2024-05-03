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

    engine.run();

}
