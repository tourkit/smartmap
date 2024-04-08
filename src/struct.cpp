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

Struct::~Struct(){

    structs.erase(this);

    // delete typed() a.k.a Data members
    for (auto x : members) if (x->typed()) delete x;

}


Struct::Struct(std::string name, uint32_t quantity) : Member(name) {

    structs.insert(this);

    quantity_v = quantity;

}


Struct& Struct::remove(Member& m) {

    PLOGV << name() << " remove " << m.name();

    pre_change();

    auto it = std::find( members.begin(), members.end(), &m );

    if (it == members.end()) { PLOGV << "no find "<< m.name(); return *this; }

    size_v -= members.back()->footprint_all();

    members.erase(it);

    update();

    post_change();

    return *this;

}




Struct& Struct::add(Member& m) {

    PLOGV << name() << " add " << m.name();

    pre_change();

    members.push_back(&m);

    size_v += members.back()->footprint_all();

    update();

    post_change();

    return *this;

}

Struct& Struct::add(const char* name) {

    for (auto s : owned) if (!strcmp(name,s->name().c_str())) { add(*s); return *this; }

    PLOGW << " noadd" << name; return *this;

}

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

void Struct::update() {

    size_v = 0;

    for (auto &m : members) size_v += m->footprint_all();

    PLOGV << name();

    Member::update();

}

uint32_t Struct::size() {

    if (

        members.size() == 1 &&
        members[0]->typed()&&
        !members[0]->name().length()

    ) return members[0]->size();

     return size_v;

}


std::type_index Struct::type()  { if (typed()) { return members[0]->type(); } return typeid(Struct); }


Member* Struct::copy(Member* x)  {

    if (!x) x = new Struct(name_v+"bkp");

    return Member::copy(x);

}


std::string Struct::print(int recurse) {

    std::string str = "struct " + camel(name())  + " {";

    for (auto m : members) {

        str += " ";

        if (!m->typed()) if (recurse) { str += m->print(recurse-1);} else {str += camel(m->name()); }

        else str += m->type_name();

        str += " " + lower(m->name());

        if (m->quantity() > 1) str += "[" + std::to_string(m->quantity()) + "]";

        str += ";";

    }

    if (stride()) for (int i = 0; i < stride()/sizeof(float); i++) {

        str += " ";
        str += (members[0]->type() == typeid(int) ? "int" : "float");
        str += " stride";
        str += std::to_string(i) + ";";

    }

    str += " }";

    return str;

}

void Struct::hard_delete() {

    auto t_members = members;

    for (auto &m : t_members) {

        m->hard_delete();

        if (!m->typed()) delete m;


    }

}
