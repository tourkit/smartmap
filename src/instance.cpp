#include "instance.hpp"
#include "struct.hpp"
#include "buffer.hpp"

bool Instance::exist(){

    if (offset == -1) return false;

    return true;

}

Instance Instance::operator[](std::string name) {

    auto offset = this->offset;

    Member* found = nullptr;

    for (auto &m : member->members) {

        if (!(strcmp(m->name().c_str(),name.c_str()))) {

            found = m;

            break;

        }

        offset += m->footprint_all();

    }

    if (!found) PLOGE << "\"" << name << "\" does not exist in " << member->name();

    return Instance{buff,offset,found};

}
Instance Instance::operator[](int id) {

    auto offset = this->offset;
    auto member = this->member;

    if (id >= member->members.size()) {PLOGE << id << "exceed"; exit(0);}

    for (int i = 0 ; i < id; i ++ ){

        auto &m = member->members[i];

        offset += m->footprint_all();

    }

    member = member->members[id];

    return Instance{buff,offset,member};

}

Instance Instance::eq(int id) {

    if (!member || id >= member->quantity()) {PLOGE<<"Not that many quantity exist";return *this;}

    uint32_t new_offset = offset + member->footprint() * (id-eq_id);

    eq_id = id;

    return Instance{buff, new_offset ,member};

}

void Instance::setDefault(Member* toset, int offset) {

    if (!toset) toset = member;

    for (auto x : toset->members) {

        setDefault(x, offset);

        offset += x->footprint_all();

    }

    if (toset->default_val_ptr) {

        // PLOGD << "------------>set " << toset->name() << " to default " << *(float*)toset->default_val_ptr << " @ " << offset;

        memcpy(data()+offset, toset->default_val_ptr, toset->size());

    }

}

Instance Instance::push(void* ptr, size_t size) {

    PLOGV << "new " << member->name() << " in " << buff->name() ;

    member->quantity(member->quantity()+1);

    auto inst = eq(member->quantity()-1);

    if (!ptr) { inst.setDefault(); }

    else {

        if (!size) size = member->size();

        inst.set(ptr,size);

    }

    return inst;

}
