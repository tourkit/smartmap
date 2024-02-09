#include "entry.hpp"
#include "buffer.hpp"



const char* Entry::name() { return obj->s->name.c_str(); }
    
Entry::Comp::Comp(int id) : id(id) {
            
    int current = 0;

    offset = 0;

    for (auto c : obj->s->comps){

        offset += c->size;

        if (current++ == id) break;

    }

}