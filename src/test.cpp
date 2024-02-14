#include "test.hpp"
#include "log.hpp"

#include "node.hpp"

#include <cstring>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <vector>
#include <map>

#include "buffer.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "entry.hpp"
#include "vbo.hpp"

// struct Foo { virtual Foo() { PLOGD << "foo"; } };
// struct Bar : Foo { Bar() { PLOGD << "bar"; Foo(); } };



#include <string>
#include <vector>



struct Foo { Foo() { PLOGD << "foo"; } ~Foo() { PLOGD << "~ foo"; } };
struct Bar {  ~Bar() { PLOGD << "~ bar"; }  Bar(int x) { PLOGD << "bar"; }};

Test::Test(){

    Foo foo;

    TypedNode<Foo>::onadd<Bar>([](Node* a, Node* b) {
        return nullptr;
    });

    {    
    
        Node a("aaa");

        auto b = a.add(new Node("b"));

        b->addPtr<Foo>(&foo);

        Ownr<Foo> owned_foo;

        auto z  = b->addOwnr<Bar>(6);
    
    }


    // auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    // obj->s->addComp({"Position"});

    // obj->push();

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    // PLOGD << "tete";


}