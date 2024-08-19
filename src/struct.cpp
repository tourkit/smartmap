#include "struct.hpp"
#include "instance.hpp"
#include <algorithm>
#include <vector>

// STATIC
Struct& Struct::create(std::string name, uint32_t quantity) { return **owned.insert(new Struct(name, quantity)).first; }

Struct* Struct::exist(std::string name) { for (auto &s : owned) if (s->name() == name) return s; return nullptr; }

Struct& Struct::id(std::string name) {

    auto x = exist(name);

    if (x) return *x;

    return create(name);

}


Struct* Struct::isStruct() { return this; }

bool Struct::removeFromOwned(std::string name) {

    Struct* found = nullptr;

    for (auto &s : owned) if (s->name() == name) { found = s; break; }

    if (found) { owned.erase(found); delete found; return true; }

    return false;

}


// NON STATIC

Struct::~Struct(){

    structs.erase(this);

}

Struct::Struct(const Member& other) : Member(other) {


}

Struct::Struct(std::string name, uint32_t quantity) : Member(name) {

    structs.insert(this);

    this->quantity(quantity);

}


Struct& Struct::clear() {

    // PLOGV << name() << " clear " ;

    pre_change();

    size_v = 0;

    auto ms = members;

    for (auto x : ms) delete x;

    members.clear();

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



// Struct* Struct::add(std::string name) { return new Struct( next_name(name) ); }

// Struct& Struct::add(Member& m, std::string name) {

//     if (!name.length()) name = m.name();

//     auto s = add( name );

//     s->add( &m );

//     add( s );

//     return *this;

// }




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
    if (a->type() == typeid(char)) {
        ((Data<char>*)members.back())->range_from = (char) from;
        ((Data<char>*)members.back())->range_to = (char) to;
        ((Data<char>*)members.back())->default_val = (char) def;
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

    if ( ref() ) return ref()->size();

     return size_v;

}


std::type_index Struct::type()  { if (isData()) { return members[0]->type(); } return Member::type(); }
Member* Struct::copy()  { return new Struct(*this); }
