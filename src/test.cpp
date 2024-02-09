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

Test::Test(){

    auto b = engine.tree->add<Buffer>()->get();

    auto o = b->addObj(new Struct("test",{ "float", "float", "float", "float", })); // could retun some betterPtr not slave to reserve()-d

    auto e = o->push(); 

    e.m<float>(0) = .5;

}