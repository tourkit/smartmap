#include "test.hpp"
#include "log.hpp"


#include <functional>

struct Foo { 
    
    Foo(std::function<void()> fx) : fx(fx) { PLOGD << "foo"; } 

    std::function<void()> fx;
    
    
};

auto foo = Foo([](){

    static int x = 6;

});

Test::Test() {



    


    // auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    // obj->s->addComp({"Position"});

    // obj->push();

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    // PLOGD << "tete";

}