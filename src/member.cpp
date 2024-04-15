#include "member.hpp"
#include "struct.hpp"


#include "log.hpp"
#include "buffer.hpp"
#include "struct.hpp"
#include <cstdint>
#include <unordered_set>
#include <typeindex>
#include <cstdint>


Member::~Member() {

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);
    PLOGV << "~" << name();// << " ( &" + ss.str() + " )";

}

Member::Member(std::string name_v) {

    name(name_v);

    if (!name_v.length()) quantity_v = 0; // ??????? if typed

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);

    PLOGV << "#" << name();// << " ( &" + ss.str() + " )" ;

}

Member::Member(const Member& other)
 :

    range_from_ptr(other.range_from_ptr),
    range_to_ptr(other.range_to_ptr),
    default_val_ptr(other.default_val_ptr),
    is_striding(other.is_striding) ,
    quantity_v( other.quantity_v ) ,
    name_v(other.name_v) ,
    members(other.members) ,
    size_v( other.size_v )

 {

    for (auto &m : members) m = m->copy();

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);

    PLOGV << "BKP#" << name();// << " ( &" + ss.str() + " )" ;

}


std::set<Member*> Member::getTop(bool z) {

    std::set<Member*> owners, out;

    for (auto x : structs) if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end()) owners.insert( x );

    if (!owners.size()) { if (!z) return {}; out.insert(this); return out; }

    for (auto owner : owners) {

        auto top = owner->getTop(true);

        out.insert(top.begin(), top.end());

    }

    return out;

}


void Member::update() { for (auto a : structs) for (auto &m : a->members) if (m == this) a->update(); }

void Member::name(std::string name_v) { this->name_v = name_v; }

std::string Member::name() { if (name_v.length()) return name_v; return "parentName" ; }

uint32_t Member::size() { return 0; }

uint32_t Member::footprint() { if (striding()) return nextFactor2(size(),16);  return size(); }

uint32_t Member::stride() { return (footprint()-size()); }

void Member::quantity(uint32_t quantity_v) {

    pre_change();

    this->quantity_v = quantity_v;

    update();

    post_change();

}

uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i * footprint(); }

uint32_t Member::footprint_all() { return eq( quantity_v ); }

void Member::striding(bool is_striding){ this->is_striding = is_striding; update(); }

bool Member::striding() { return is_striding; }

std::type_index Member::type() { return typeid( *this ); }

std::string Member::type_name() {

    if (type() == typeid(glm::vec2)) return "vec2";

    if (type() == typeid(glm::vec3)) return "vec3";

    if (type() == typeid(glm::vec4)) return "vec4";

    if (type() == typeid(float)) return "float";

    if (type() == typeid(int)) return "int";

    if (type() == typeid(uint32_t)) return "uint";

    if (type() == typeid(char)) return "char";

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

Member* Member::copy() { return new Member(*this); }

bool Member::typed() { return false; }

std::string Member::print(int recurse) {

    std::string str = type_name() + " " + camel(name())  + ";";

    return str;

}

void Member::each(std::function<void(Member*, uint32_t)> f, uint32_t offset) {

    auto offset_ = offset;

    for (auto m : members) {

        m->each(f, offset_);

        offset_ += m->footprint_all();

    }

    f(this, offset);

}

void Member::pre_change() {

    for (auto x : getTop())  x->pre_change();

}

void Member::post_change(std::vector<Member*> added) {

    for (auto x : getTop()) x->post_change(added);

}
