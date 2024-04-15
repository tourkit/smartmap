/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"

int main() {

    // logger.cout(true);

    engine.init();

    auto &dc = *engine.stack->addOwnr<DrawCall>();

    auto m_node = dc.add((*engine.models)[0]);

    m_node->add((*engine.effectors)[0]);

    auto x = Struct::exist("argb") ;

    PLOGD << *(float*)x->members[0]->default_val_ptr;
    PLOGD << x->members[0]->name();


    // m_node->is_a<Model>()->quantity(2);

    // m_node->add((*engine.effectors)[3]);
    // m_node->add((*engine.effectors)[1]);

    engine.gui->editors.push_back(new EditorWidget());
    engine.gui->editors.back()->locked = true;
    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
}


// Struct::OnAdd(q) { for(q) push() }

// Struct::OnAdd need to trigger parent for push

// set default value

// push is not called onadd effector to model

 // onadd effector to model is like add struct (i think pb is :  inplyed q1 wich is not push though)

 // setDefault shopuld be in Struct::add(Member& m)
