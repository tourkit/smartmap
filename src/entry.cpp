#include "entry.hpp"
#include "buffer.hpp"


#include "object.hpp"

Entry::Comp::Comp(int id, Entry* entry) : id(id), entry(entry) { 

    offset = 0;
    int curr = 0;
    for (auto c : entry->obj->s->comps) {
        if (curr++ >= id) break;
        offset += c->size;
    }

}

char* Entry::Comp::data() { return entry->data()+offset; }

Entry::Comp::Member Entry::Comp::operator[](const char* name) { 

    int id = 0;

    for (auto m : entry->obj->s->comps[id]->members) { if (!(strcmp(m.name.c_str(),name))) { break;} id++; }

    return (*this)[id];

}

Entry::Comp::Comp::Member Entry::Comp::operator[](int id) { 

    int member_offset = 0;
    int current = 0;
    for (auto m : entry->obj->s->comps[this->id]->members) {
        if (current++ == id) break;
        member_offset += m.size;
    }

    return Member{data()+member_offset};
    
}


Entry::Comp Entry::operator[](int id) { 
    
    return Comp{id,this}; 
    
}

Entry::Comp Entry::operator[](const char* name) { 
    
    int id = 0;

    for (auto c : obj->s->comps) { if (!(strcmp(c->name.c_str(),name))) { break;} id++; }

    return (*this)[id];

}

char*  Entry::data() { return obj->data(id); }