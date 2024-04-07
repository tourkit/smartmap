#include "struct.hpp"

// STATIC
Struct& Struct::create(std::string name, uint32_t quantity) { return **owned.insert(new Struct(name, quantity)).first; }

Struct* Struct::exist(std::string name) { for (auto &s : owned) if (s->name() == name) return s; return nullptr; }

Struct& Struct::id(std::string name) {

    auto x = exist(name);

    if (x) return *x;

    return create(name);

}

void Struct::clear() { for ( auto s : owned ) delete s;  }

bool Struct::destroy(std::string name) {

    Struct* found = nullptr;

    for (auto &s : owned) if (s->name() == name) { found = s; break; }

    if (found) { owned.erase(found); delete found; return true; }

    return false;

}


// NON STATIC

Struct::Struct(std::string name, uint32_t quantity) : Member(name) { quantity_v = quantity; if (!name.length()) quantity_v = 0; }


Struct& Struct::add(Struct& s) { Member::add(&s); return *this; }

Struct& Struct::add(const char* name) {

    for (auto s : owned) if (!strcmp(name,s->name().c_str())) { add(*s); return *this; }

    PLOGW << " noadd" << name; return *this;

}

Struct& Struct::remove(Struct& s) { Member::remove(&s); return *this; }


Struct& Struct::add(Member* m) { Member::add(m); return *this; }

Struct& Struct::range(float from, float to, float def) {

    auto a = members.back();
    if (typeid(*a) == typeid(Data<float>)) {
        ((Data<float>*)members.back())->range_from = from;
        ((Data<float>*)members.back())->range_to = to;
        // PLOGD << " ----- is : float";
    }

    if (typeid(*a) == typeid(Data<uint32_t>)) {
        ((Data<uint32_t>*)members.back())->range_from = (uint32_t) from;
        ((Data<uint32_t>*)members.back())->range_to = (uint32_t) to;
        // PLOGD << " ----- is : uint32_t";
    }

    return *this;

}
std::type_index Struct::type()  { if (typed()) { return members[0]->type(); } return typeid(Struct); }


Member* Struct::copy(Member* x)  {

    if (!x) x = new Struct(name_v);

    return Member::copy(x);

}
