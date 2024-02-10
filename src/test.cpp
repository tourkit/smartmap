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

    auto buffer = engine.tree->add<Buffer>();

    buffer->select();

    auto foo = buffer->get()->addObj(new Struct("Foo", {"float"}));
    auto bar = buffer->get()->addObj(new Struct("Bar", {"float"}));

    float f[3] = {1,2,3};

    for (int i = 0 ; i < 4; i++) {

        auto vertice = foo->push();

        vertice[0][0].set<float>(4);

    }
    
    for (int i = 0 ; i < 2; i++) {

        auto indices = bar->push(&f[0]); // pushing to second obj fucks first 

        indices[0][0].set<float>(5);
    }


    // add Objects child to Buffer w/ child Comp


    /// refaire obj ofgfset bxcp plus souvent ( update )

}