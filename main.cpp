/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "log.hpp"
#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"
#include "drawcall.hpp"
#include "layer.hpp"



int main() {

    engine.init();

    engine.open("./project.json");

    auto wrap = new Wrappy();

    wrap->addEffector((*engine.effectors)["atlas"]->is_a<Effector>());
    wrap->addEffector((*engine.effectors)["burst"]->is_a<Effector>());
    wrap->addEffector((*engine.effectors)["flower"]->is_a<Effector>());
    wrap->addEffector((*engine.effectors)["gradient"]->is_a<Effector>());

    auto wrap_ = engine.effectors->addPtr<Effector>(wrap);

    engine.stack->childrens[0]->childrens[0]->add(wrap_);
    engine.stack->childrens[0]->select();

    engine.run();

}
