/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"

#include "drawcall.hpp"


int main() {

    logger.cout(true);

    engine.init();

    auto &dc = *engine.stack->addOwnr<DrawCall>();

    auto m_node = dc.add((*engine.models)[0]);

    m_node->add((*engine.effectors)[0]);
    m_node->add((*engine.effectors)[1]);

    // auto &m = *m_node->is_a<Model>();
    // m.add((*engine.effectors)[0]->is_a<File>());
    // m.add((*engine.effectors)[1]->is_a<File>());

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();

}
