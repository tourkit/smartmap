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

    for (auto s : structs) {

        if (std::find(s->members.begin(), s->members.end(), this) != s->members.end()) {

            s->remove(*this);

        }

    }

    // delete typed() a.k.a Data members
    for (auto x : members) {
        if (x->typed())
            delete x;

    }
}

Struct::Struct(const Member& other) : Member(other) {


}

Struct::Struct(std::string name, uint32_t quantity) : Member(name) {

    structs.insert(this);

    this->quantity(quantity);

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

    for (auto x : members) if (!strcmp( x->name().c_str(), m.name().c_str() )) { m.name(m.name()+ "_copy"); break ; }

    pre_change();



    members.push_back(&m);

    size_v += members.back()->footprint_all();

    update();

    post_change({&m});

    return *this;

}

Struct& Struct::add(const char* name) {

    for (auto s : owned) if (!strcmp(name,s->name().c_str())) { add(*s); return *this; }

    PLOGE << " noadd" << name; return *this;

}

Struct& Struct::range(float from, float to, float def) {

    auto a = members.back();
    if (a->type() == typeid(float)) {
        ((Data<float>*)members.back())->range_from = from;
        ((Data<float>*)members.back())->range_to = to;
        ((Data<float>*)members.back())->default_val = def;
    }

    if (a->type() == typeid(uint32_t)) {
        ((Data<uint32_t>*)members.back())->range_from = (uint32_t) from;
        ((Data<uint32_t>*)members.back())->range_to = (uint32_t) to;
        ((Data<uint32_t>*)members.back())->default_val = (uint32_t) def;
    }
    if (a->type() == typeid(int)) {
        ((Data<int>*)members.back())->range_from = (int) from;
        ((Data<int>*)members.back())->range_to = (int) to;
        ((Data<int>*)members.back())->default_val = (int) def;
    }

    if (a->type() == typeid(glm::vec2)) {

        ((Data<glm::vec2>*)members.back())->range_from = glm::vec2({from,from});
        ((Data<glm::vec2>*)members.back())->range_to = glm::vec2({to,to});
        ((Data<glm::vec2>*)members.back())->default_val = glm::vec2({def,def});

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

Member* Struct::copy()  { return new Struct(*this); }

std::string Struct::print(int recurse) {

    // std::string name = this->name();
    // std::replace(name.begin(), name.end(), ' ', '_');

    // if (!members.size()) return "";

    std::string str;

    for (auto m : members) {

        std::string m_str;

        // if (!m->members.size() && !m->quantity()) continue;

        if (!m->typed()) if (recurse) { m_str += m->print(recurse-1); if (!m_str.length()) continue; } else {m_str += camel(m->name()) ; }

        else m_str += m->type_name();

        m_str += " " + lower(m->name());

        if (m->quantity() > 1) m_str += "[" + std::to_string(m->quantity()) + "]";
        // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(m);
        // str  += " ( &" + ss.str() + " )";
        m_str += "; ";

        str+= m_str;

    }

    if (stride()) for (int i = 0; i < stride()/sizeof(float); i++) {

        str += " ";
        str += (members[0]->type() == typeid(int) ? "int" : "float");
        str += " stride";
        str += std::to_string(i) + ";";

    }

    if (!str.length()) return "";

    return "struct " + camel(name())  + " { " + str + "}";

}

void Struct::hard_delete() {

    auto t_members = members;

    for (auto &m : t_members) {

        m->hard_delete();

        if (!m->typed()) delete m;

        // members erase m
        members.erase(std::find(members.begin(), members.end(), m));

    }

}
