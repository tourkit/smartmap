/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"

struct Protu {};

int main() {

    auto zxx = new Protu;

    logger.cout(true);

    engine.init();

    auto &dc = *engine.stack->addOwnr<DrawCall>();

    Node* m_node = dc.add((*engine.models)[0]);

    for (int i = 0 ; i < 2; i++) {

        auto x = m_node->add((*engine.effectors)[3]);
        // auto x = m_node->addOwnr<Protu>();
        delete x;

    }

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();

}

// text x100 jsut eporu voir;

// redo copy !

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
