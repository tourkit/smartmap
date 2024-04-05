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

    // Struct x("x");

    // {
    //     Struct a("a");
    //     Struct b("b");
    //     a.add(b);
    //     b.add<float>("c").add<glm::vec3>("d");
    //     b.add(x);
    //     PLOGD << "----------------";

    // }
    // exit(0);

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


// text x100 jsut eporu voir;

// redo copy ! // gota do bkp member/Struct/Buffer on Node remove (thus on delete)

// finish remap
// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push
