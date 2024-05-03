/*

    SMARTMAP (beta)

                        */

#include "drawcall.hpp"

#include "engine.hpp"

int main() {

    engine.init();

    logger.cout();

    auto l1 = engine.stack->addOwnr<Layer>();

    // NODE<Lay>::onchange( [](Node*node, Lay*lay){ PLOGD <<"-"<<lay->s.print_recurse() <<"-"; });

    // auto m1 = l1->add(fm1);

    // auto e1 = m1->add(fe1);

    auto q1 = l1->add(engine.models->child("quad"));

    q1->add( engine.effectors->child("argb") );

    engine.run();

}
