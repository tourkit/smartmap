#include "instance.hpp"
#include "struct.hpp"
#include "buffer.hpp"

bool Instance::exist(){ 

    if (offset == -1) return false;

    return true;

}

Instance Instance::operator[](std::string name) { 

    if  (offset < 0) return Instance{buff,offset-1,member};

    Member* found = nullptr;

    if (!member) {PLOGW << "BUGGY";exit(0);}

    for (auto &m : member->members) { 

        if (!(strcmp(m->name().c_str(),name.c_str()))) {
        
            found = m;

            break;
        
        }
        
        offset += m->footprint()*m->quantity();
        
    }

    if (!found) { offset = -1; PLOGW << "\"" << name << "\" does not exist"; }

    return Instance{buff,offset,found};

}
Instance Instance::operator[](int id) { 

    if  (offset < 0) return Instance{buff,offset-1,member};

    if (!member) {PLOGW << "BUGGY";exit(0);}

    if (id >= member->members.size()) {PLOGW << "WAWWA"; exit(0);}

    for (int i = 0 ; i < id-1; i ++ ){

        auto &m = member->members[i];

        offset += m->footprint()*m->quantity();
        
    }

    return Instance{buff,offset,member->members[id]};

}

Instance& Instance::eq(int id) {

    if (!member || id >= member->quantity()) return *this;
    
    id = id-this->id;

    offset += member->footprint() * id;
    
    return *this;

}