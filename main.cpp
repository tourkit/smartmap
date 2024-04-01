/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "callbacks.hpp"
#include "drawcall.hpp"

int main() {

    logger.cout(true);

    engine.init();

    auto &dc = *engine.stack->addOwnr<DrawCall>();

    Node* m_node = dc.add((*engine.models)[0]);


    engine.gui->editors.push_back(new EditorWidget());

    engine.run();

}

// text x100 jsut eporu voir;

// redo copy !

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
