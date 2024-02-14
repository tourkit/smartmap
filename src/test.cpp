#include "test.hpp"
#include "log.hpp"



// struct Foo { virtual Foo() { PLOGD << "foo"; } };
// struct Bar : Foo { Bar() { PLOGD << "bar"; Foo(); } };



#include <string>
#include <vector>



struct Foo { Foo() { PLOGD << "foo"; } ~Foo() { PLOGD << "~ foo"; } };
struct Bar {  ~Bar() { PLOGD << "~Bar"; }  Bar(int x) { PLOGD << "bar"; }};

Test::Test(){


    // auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    // obj->s->addComp({"Position"});

    // obj->push();

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    // PLOGD << "tete";


}