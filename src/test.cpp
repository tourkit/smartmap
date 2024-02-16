#include "test.hpp"
#include "log.hpp"

struct Foo { Foo() { PLOGD << "foo"; } };
// struct Bar : Foo { Bar() { PLOGD << "bar"; Foo(); } };


template <typename T>
struct Ownr;

struct XXX {

    template <typename U>
    static Ownr<U>* y() { return nullptr; }

};

template <typename T>
struct Ownr { /* ... */ };




Test::Test() {
    
    XXX::y<Foo>();

    // auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    // obj->s->addComp({"Position"});

    // obj->push();

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    // PLOGD << "tete";

}