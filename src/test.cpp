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

Test::Test(){

    auto a = engine.tree->add<Buffer>();
    engine.editorw.selected = a;
    auto b = a->get();

    auto o = b->addObj(new Struct("test",{"Position", "float"})); // could retun some betterPtr not slave to reserve()-d

    auto e = o->push(); 

    e[0][0].set<float>(100);
    e[0][1].set<float>(200);

    o->push(); 

}