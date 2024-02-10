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

    auto buffer = engine.tree->add<Buffer>()->get();

    auto obj = buffer->addObj(new Struct("test",{"Position", "Size"}));

    auto indices = obj->push();
    
    indices[0][0].set<float>(3);
    indices[0][1].set<float>(4);
    indices[1][0].set<float>(5);

    //set comp above 0 is wrong

}