#include "test.hpp"
#include "log.hpp"

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

Test::Test(){


    auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    obj->s->addComp({"Position"});
    obj->push();


}