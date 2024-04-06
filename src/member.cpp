#include "member.hpp"


#include "log.hpp"
#include "buffer.hpp"
#include "struct.hpp"
#include <cstdint>
#include <unordered_set>
#include <typeindex>


Member::~Member() {

    static Buffer* bkp = nullptr;

    // remove from pool
    pool.erase(this);

    // // remove from any Members in the pool
    for (auto m : pool) {

        if (std::find(m->members.begin(), m->members.end(), this) != m->members.end()) {

            m->remove(this);

        }

    }

    // delete typed() a.k.a Data members
    for (auto x : members) if (x->typed()) delete x;

    PLOGV << "~" << name();

}

void Member::destroy() {


}

Member* Member::add(Member* m) {

    PLOGV << "add " << m->name() << " to " << name();

    members.push_back(m);

    size_v += members.back()->footprint_all();

    update();

    return this;

}

Member::Member(std::string name_v) {

    name(name_v);

    PLOGV << "#" << name();

    pool.insert(this);

}


void Member::update() {

    size_v = 0;

    for (auto &m : members) size_v += m->footprint_all();

    PLOGV << name();

    for (auto a : pool) {

        for (auto &m : a->members) if (m == this) a->update();

    }

}

uint32_t Member::size() {

        if (members.size() == 1 && members[0]->typed()) { return members[0]->size(); }

        return size_v;

}

uint32_t Member::footprint() {

    if (striding()) return nextFactor2(size_v,16);

    return size_v;

}

void Member::quantity(uint32_t quantity_v) { this->quantity_v = quantity_v; update(); }

uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i*footprint(); }

uint32_t Member::footprint_all() { return footprint() * quantity_v; }

std::type_index Member::type() { return typeid(*this); }

const char* Member::type_name() {

    if (type() == typeid(glm::vec2)) return "vec2";

    if (type() == typeid(glm::vec3)) return "vec3";

    if (type() == typeid(glm::vec4)) return "vec4";

    if (type() == typeid(float)) return "float";

    if (type() == typeid(int)) return "int";

    if (type() == typeid(uint32_t)) return "uint";

        // return "Sampler2D";

    return "unknown";

}

uint8_t Member::count() {

    // if (type() == typeid(Struct)) { int x = 0; for (auto m : members) { x += m.count()}; ); return x; }

    if (type() == typeid(glm::vec2)) return 2;

    if (type() == typeid(glm::vec3)) return 3;

    if (type() == typeid(glm::vec4)) return 4;

    return 1;

}

void Member::name(std::string name_v) { this->name_v = name_v; }

std::string Member::name() { return name_v; }

Member* Member::any() { return this; }

uint32_t Member::stride() { return (footprint()-size()); }

void Member::striding(bool is_striding){ this->is_striding = is_striding; update(); }

bool Member::striding() { return is_striding; }

Member* Member::copy(Member* x) {

    if(!x) x = new Member(name_v);

    this->is_striding = is_striding;

    x->quantity_v = quantity_v;

    x->members = members;

    for (auto &m : x->members) m = m->copy();

    x->size_v = size_v;
    return x;

}

bool Member::typed() { return false; if (members.size() == 1 && members[0]->name_v.length()) return true; return false; }

std::string Member::print(int recurse) {

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


void Member::hard_delete() {

    for (auto &m : members) {

        m->hard_delete();

        if (!m->typed()) {

            auto to_delete =  m;

            delete m;

            members.erase(std::remove(members.begin(), members.end(), to_delete), members.end());

        }

    }

}

Member* Member::remove(Member* m) {

    auto it = std::find( members.begin(), members.end(), m );

    if (it == members.end()) { PLOGV << "no find "<< m->name(); return this; }

    size_v -= members.back()->footprint_all();

    members.erase(it);

    update();

    return this;

}
