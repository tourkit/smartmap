/*

    SMARTMAP (beta)

                        */

#include "buffer.hpp"
#include "log.hpp"
#include "engine.hpp"


// struct Foo { Foo(int x) { } };
// struct Bar {

//      std::unique_ptr<Foo> foo;

//     Bar(Foo* foo) { this->foo = std::make_unique<Foo>(foo); }

// };

int main() {

    // Bar b(5);


    logger.cout();

    Buffer buffy;

    auto obj = buffy.addObj(new Struct("Index",{"Vertex", "Vertex", "Vertex"}));
    obj->is_stride = false;

    auto e1 = obj->push();

    e1[0].set<uint32_t>(111);
    e1[1].set<uint32_t>(222);
    e1[2].set<uint32_t>(333);

    PLOGD<<"second";

    auto e2 = obj->push();

    e2[0].set<uint32_t>(444);
    e2[1].set<uint32_t>(555);
    e2[2].set<uint32_t>(666);
    
    PLOGD << e1[0][0].get<uint32_t>();
    PLOGD << e1[1][0].get<uint32_t>();
    PLOGD << e1[2][0].get<uint32_t>();
    PLOGD << e2[0][0].get<uint32_t>();
    PLOGD << e2[1][0].get<uint32_t>();
    PLOGD << e2[2][0].get<uint32_t>();

    PLOGD << (*obj->entrys[0])[0][0].get<uint32_t>();

    PLOGD << obj->entrys[0]->data();
    PLOGD << obj->entrys[1]->data();

    engine.init();
    engine.gui->editors.push_back(new EditorWidget());

    engine.tree->addPtr<Buffer>(&buffy)->select();

    engine.run();

}