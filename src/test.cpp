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

Test::Test(){

    auto x = engine.tree->add<Buffer>();
    engine.editorw.selected = x;
    auto buffer = x->get();

    auto obj = buffer->addObj(new Struct("test", {"U32Vec3"}));

    
    auto indices = obj->push();


    indices[0][0].set<uint32_t>(3);
    indices[0][1].set<uint32_t>(4);
    indices[0][2].set<uint32_t>(5);

    // add Objects child to Buffer w/ child Comp

}