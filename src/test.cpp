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


Test::Test(){
    
   auto b = engine.tree->add<Buffer>()->get();

   

}