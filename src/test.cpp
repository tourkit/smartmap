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

    auto o = b->addObj(new Struct("test",{"2chars"})); // could retun some betterPtr not slave to reserve()-d

    auto e = o->push(); 

    e[0][0].set<uint8_t>(100);
    e["2chars"]["char2"].set<uint8_t>(200);
    PLOGD << "oooo";
    // o->push(); 

}