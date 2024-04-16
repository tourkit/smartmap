/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "log.hpp"
#include "boilerplate.hpp"
#include "drawcall.hpp"


    struct Foo { int x; Foo(int x) : x(x) { PLOGD << "#Foo "<<x; } ~Foo() { PLOGD << "~Foo "<<x; } };

    std::vector<Foo> foos ;

int main() {


    // foos.emplace_back(1);

    // foos.emplace_back(2);

    // // foos.resize(1);

    // exit(0);

    engine.init();

    logger.cout();

    // VBO vbo;

    // vbo.add(new File("assets/models/quad.obj"));


    // vbo.add(new File("assets/models/quad1.obj"));


    // PLOGD << engine.dynamic_ubo.print(2);



    auto &dc = *engine.stack->addOwnr<DrawCall>();

    auto m_node = dc.add((*engine.models)[0]);

    m_node = dc.add((*engine.models)[0]);

    // // m_node->is_a<Model>()->quantity(2);

    // m_node->add((*engine.effectors)[3]);
    // m_node->add((*engine.effectors)[1]);
    // m_node->add((*engine.effectors)[0]);

    m_node->select();

    engine.gui->editors.push_back(new EditorWidget());
    engine.gui->editors.back()->locked = true;
    engine.gui->editors.back()->selected = engine.debug;
    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
}


// editor slider per model

// multiple quad ( easy way is adding number on nome())

// movement


//artnet

//remap
