#include "instance.hpp"
#include "buffer.hpp"


#include "object.hpp"

Instance::Comp::Comp(int id, Instance* instance) : id(id), instance(instance) { 

    offset = 0;
    int curr = 0;
    for (auto c : instance->obj->s->comps) {
        if (curr++ >= id) break;
        offset += c->size;
    }

}

char* Instance::Comp::data() { return instance->data()+offset; }

Instance::Comp::Member Instance::Comp::operator[](const char* name) { 

    int id = 0;

    for (auto m : instance->obj->s->comps[id]->members) { if (!(strcmp(m.name.c_str(),name))) { break;} id++; }

    return (*this)[id];

}

Instance::Comp::Comp::Member Instance::Comp::operator[](int id) { 

    int member_offset = 0;
    int current = 0;
    for (auto m : instance->obj->s->comps[this->id]->members) {
        if (current++ == id) break;
        member_offset += m.size;
    }

    return Member{data()+member_offset};
    
}


Instance::Comp Instance::operator[](int id) { 
    
    return Comp{id,this}; 
    
}

Instance::Comp Instance::operator[](const char* name) { 
    
    int id = 0;

    for (auto c : obj->s->comps) { if (!(strcmp(c->name.c_str(),name))) { break;} id++; }

    return (*this)[id];

}

char*  Instance::data() { return obj->data(id); }