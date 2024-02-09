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

    auto b = engine.tree->add<Buffer>()->get();

    auto o = b->addObj(new Struct("test",{ "Position", "float", })); // could retun some betterPtr not slave to reserve()-d

    o->push(); 
    auto e = o->push(); 
    o->push(); 

    e["Position"]["y"].set<float>(2);

    // add curent selector in buffer editor


}