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

    for (int i = 0 ; i < 100; i++) {

        auto m_node = dc.add((*engine.models)[0]);

        delete m_node;

    }

    // m_node->add((*engine.effectors)[2]);
    // m_node->add((*engine.effectors)[0]);
    // m_node->add((*engine.effectors)[1]);

    engine.gui->editors.push_back(new EditorWidget());

    // engine.

    engine.run();

}


// create a gui::delete pool;
