#include "entry.hpp"
#include "buffer.hpp"



const char* Entry::name() { return obj->s->name.c_str(); }
    
Entry::Comp::Comp(int id) : id(id) {
            

}