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

    if (!member || id >= member->quantity()) {PLOGE<<"WAWA";return *this;}

    uint32_t new_offset = offset + member->footprint() * (id-eq_id);

    eq_id = id;

    return Instance{buff, new_offset ,member};

}
