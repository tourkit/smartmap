/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "drawcall.hpp"
#include "engine.hpp"



int main() {



    logger.cout(true);

    engine.init();

    auto &dc = *engine.stack->addOwnr<DrawCall>();

    Node* m_node;
    m_node = dc.add((*engine.models)[0]);
    m_node->add((*engine.effectors)[3]);
    m_node->add((*engine.effectors)[0]);

    // auto &bkp = engine.dynamic_ubo.copy();
    // bkp["quad"]["rectangle"]["pos"].set<glm::vec2>(glm::vec2(0.5,0.5));
    // PLOGD<< bkp.print(2);
    // bkp.printData();
    // engine.dynamic_ubo.remap( bkp );
    // PLOGD<< engine.dynamic_ubo.print(2);
    // engine.dynamic_ubo.printData();

    // bkp.hard_delete();
    // PLOGD <<"-";
    // delete &bkp;
    // PLOGD <<"-";
    // exit(0);

    engine.gui->editors.push_back(new EditorWidget());
    engine.gui->editors.back()->locked = true;
    engine.gui->editors.push_back(new EditorWidget());

    engine.run();

}

// maker update list first || do a modif(add,remove,move,resize) pool at new frame ...


// redo copy ! // gota do bkp Buffer on Node Model and Effector and DrawCall remove (thus on delete)

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
