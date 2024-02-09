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

   auto foo = b->addObj(new Struct("Foo",{std::string("float")}));
   auto bar = b->addObj(new Struct("Bar",{std::string("float")}));

   // foo->push();

   // check member::size()
   // check component::size()
   // check struct::size()
   // check object::size()
   // check buffer::size()

}