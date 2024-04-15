/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"
#include "texture.hpp"


int main() {


    // VBO vbo;

    // PLOGD << vbo.print(2);

    // auto v = vbo[0].push()[0];

    // v[0].set<std::array<uint32_t,2>>({20,21});
    // v[1].set<std::array<uint32_t,2>>({22,23});
    // v[2].set<int>(24);

    // v = vbo[0].push()[0];

    // v[0].set<std::array<int,2>>({20,21});
    // v[1].set<std::array<int,2>>({22,23});
    // v[2].set<int>(24);


    // v = vbo[1].push()[0];

    // v[0].set<int>(10);
    // v[1].set<int>(11);
    // v[2].set<int>(12);

    // v = vbo[1].push()[0];

    // v[0].set<int>(20);
    // v[1].set<int>(21);
    // v[2].set<int>(22);

    // vbo.printData();


    // exit(0);

    // auto &medi = Struct::create("medi",0).add<uint32_t>("a").add<uint32_t>("b");

    // Buffer bbb;
    // bbb.add(medi);

    // std::array<uint32_t,10> a = {1,2,3,4,5,6,7,8,9,10};
    // bbb[0].push(&a);
    // bbb[0].push(&a[2]);
    // bbb[0].push(&a[4]);

    // bbb.printData();


    // exit(0);

    engine.init();

    logger.cout(true);

    engine.tree->addOwnr<Texture>("assets/medias/boy.jpg")->active(true)->onrun([](Node* n){ n->is_a<Texture>()->bind(); });

    auto &dc = *engine.stack->addOwnr<DrawCall>();

    auto m_node = dc.add((*engine.models)[0]);

    // m_node->is_a<Model>()->quantity(2);

    // m_node->add((*engine.effectors)[3]);
    // m_node->add((*engine.effectors)[0]);
    // m_node->add((*engine.effectors)[1]);

    engine.gui->editors.push_back(new EditorWidget());
    engine.gui->editors.back()->locked = true;
    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
}


// then
// on Buffer:: add (member) { set defeualt } // do range // do default // on push OnAdd(q) { for(q) push() }
