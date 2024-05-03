/*

    SMARTMAP (beta)

                        */

#include "member.hpp"
#include "struct.hpp"
#include "node.hpp"
#include "callbacks.hpp"

#include "engine.hpp"
#include "callbacks.hpp"
#include "vbo.hpp"
#include "drawcall.hpp"

#include <memory>
#include <map>


int main() {

engine.init();

logger.cout(true);

auto l1 = engine.stack->addOwnr<Layer>();

// NODE<Lay>::onchange( [](Node*node, Lay*lay){ PLOGD <<"-"<<lay->s.print_recurse() <<"-"; });

// auto m1 = l1->add(fm1);

// auto e1 = m1->add(fe1);

engine.run();


}
