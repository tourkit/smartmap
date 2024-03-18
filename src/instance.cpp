#include "instance.hpp"
#include "struct.hpp"
#include "buffer.hpp"

bool Instance::exist(){ 

    if (offset == -1) return false;

    return true;

}

Instance Instance::operator[](const char* name) { 

    if  (offset < 0) return Instance{data,offset-1,member};

    Member* found = nullptr;

    if (!member) {PLOGW << "BUGGY";exit(0);}

    for (auto &m : member->members) { 

        if (!(strcmp(m->name().c_str(),name))) {
        
            found = m;

            break;
        
        }
        
        offset += m->footprint()*m->quantity;
        
    }

    if (!found) { offset = -1; PLOGW << "\"" << name << "\" does not exist"; }

    return Instance{data,offset,found};

}

Instance& Instance::eq(int id) {

    if (!member || id >= member->quantity) return *this;
    
    id = id-this->id;

    offset += member->footprint() * id;
    
    return *this;

}