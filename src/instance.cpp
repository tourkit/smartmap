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

    if (!found) PLOGE << "\"" << name << "\" does not exist";

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

    if (!member || id >= member->quantity()) {PLOGD<<"WAWA";return *this;}

    return Instance{buff,offset + member->footprint() * (id-this->id) ,member->members[0]};

}
