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

    auto obj = buffer->addObj(new Struct("test",{"Position", "float"}));
    // buffer->addObj(new Struct("a",{"int"}))->push();

    auto indices = obj->push();
    
    // auto x = indices[0];

    indices[0][0].set<float>(3);
    indices[0][1].set<float>(4);

    // indices[1][0].set<float>(5); // set comp above 0 is wrong

    // add Objects child to Buffer w/ child Comp

}