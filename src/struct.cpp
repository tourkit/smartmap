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

static bool same_name(Member* x, Member* b) {

    auto name_ = x->name();

    size_t pos = name_.find("_");

    if (pos != std::string::npos) name_ = x->name().substr(0, pos);

    return !strcmp(x->name().c_str(), b->name().c_str());

}


Struct& Struct::add(Member& m, std::string name) {

    if (!name.length()) name = m.name();

    int count = 0;

    for (auto x : members) {

        auto name_ = x->name();

        size_t pos = name_.find("_");

        int i = 1;

        if (pos != std::string::npos) {

            name_ = x->name().substr(0, pos);

            i = std::stoi(x->name().substr(pos+1));

        }

        if (!strcmp(name_.c_str(), name.c_str())) {

            if ( i > count) count =  i;

            else count++;

        }

    }

    if (count) name += "_" + std::to_string(count) ;

    auto s = new Ref( name );

    s->add( &m );

    add( s );

    return *this;

}

void Struct::add(Member* m_) {

    Member &m = *m_;

    PLOGV << name() << " add " << m.name();

    while (true) {

        bool found = false;

        for (auto x : members) {

            if (!strcmp( x->name().c_str(), m.name().c_str() )) {

                found = true;

                PLOGW << m.name() << " already exist";

                // m.name(m.name()+ "_");

                break ;

            }

        }

        if (!found) break;

    }

    pre_change();

    members.push_back(&m);

    size_v += members.back()->footprint_all();

    update();

    post_change({&m});

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

    std::string out;

    auto list = extract_definitions();

    for (auto x : list) out += x->print_recurse() + "\n";

    return out + print_recurse(recurse);

};

void Struct::hard_delete() {

    auto t_members = members;

    for (auto &m : t_members) {

        m->hard_delete();

        if (!m->typed()) delete m;

        // members erase m
        members.erase(std::find(members.begin(), members.end(), m));

    }

}
