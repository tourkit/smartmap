#include "member.hpp"
#include "struct.hpp"
#include "instance.hpp"


#include "log.hpp"
#include "buffer.hpp"
#include "utils.hpp"
#include <cstdint>
#include <unordered_set>
#include <typeindex>
#include <cstdint>
#include <memory>



Member::~Member() {

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);

    
    if (!is_copy) PLOGV << "~" << name();// << " ( &" + ss.str() + " )";

    removing.insert(this);

    // remove from other structs
    for (auto s : structs) {

        if (std::find(s->members.begin(), s->members.end(), this) != s->members.end()) {

            s->remove(*this);

        }

    }
    // remove from other structs
    for (auto s : buffers) {

        if (std::find(s->members.begin(), s->members.end(), this) != s->members.end()) {

            s->remove(*this);

        }

    }

    // delete isData() a.k.a Data members
    for (auto x : members) {

        if (x->isData() ){

            delete x;

        }

    }

    removing.erase(this);

    if (is_copy) return;
    
    // PLOGV << "~" << name();// << " ( &" + ss.str() + " )";

    
}

Member::Member(std::string name_v) {

    name(name_v);

    if (!name_v.length()) quantity_v = 0; // ??????? if typed

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);

    PLOGV << "#" << name();// << " ( &" + ss.str() + " )" ;

}


Member::Member(const Member& other)
 :

    is_striding(other.is_striding) ,
    quantity_v( other.quantity_v ) ,
    name_v(other.name_v) ,
    members(other.members) ,
    size_v( other.size_v ) ,
    ref_v(other.ref_v),
    is_copy(true)

 {

    for (auto &m : members) m = m->copy();
    // for (auto &m : members) m = std::shared_ptr<Member>(m->copy());

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);

    // PLOGV << "BKP#" << name();// << " ( &" + ss.str() + " )" ;

}

void Member::deleteData(){

    auto t_members = members;

    for (auto m : t_members) {

        
        if (m->isData()) 
            
            delete m;

        m->deleteData();
    }

}

std::set<Member*> Member::getTop(bool z) {

    std::set<Member*> owners, out;

    for (auto x : structs) if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end()) owners.insert( x );
    for (auto x : buffers) if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end()) owners.insert( x );

    if (!owners.size()) { if (!z) return {}; out.insert(this); return out; }

    for (auto owner : owners) {

        auto top = owner->getTop(true);

        out.insert(top.begin(), top.end());

    }

    return out;

}

bool Member::each(std::function<bool(Member*)> cb) {

    for (auto m : members) {

        if (!m->each(cb)) return false;

        if (!cb(m)) return false;
        
    }

    return true;

}

void Member::update() { 
    
    for (auto a : structs) 
        for (auto &m : a->members) 
            if (m == this) 
                a->update();

 }

void Member::name(std::string name_v) { this->name_v = next_name(name_v); }

std::string Member::name() { 
    if (name_v.length()) 
        return name_v; 
    
    if (ref()) 
        return ref()->name(); 
    
    return "parentName" ; 
}

Member* Member::ref() { return ref_v; }

bool Member::ref(Member* ref) { 
    ref_v = ref; 
    return true; 
}

uint32_t Member::size() { return 0; }

uint32_t Member::footprint() { if (striding()) return nextFactor2(size(),16);  return size(); }

uint32_t Member::stride() { return (footprint()-size()); }

Member* Member::quantity(uint32_t quantity_v) {

    pre_change();

    auto old = this->quantity_v;

    this->quantity_v = quantity_v;

    update();

    post_change({{this, old, (int)quantity_v-old}});

    return this;

}

uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i * footprint(); }

uint32_t Member::footprint_all() { return eq( quantity_v ); }

void Member::striding(bool is_striding){ this->is_striding = is_striding; update(); }

bool Member::striding() { return ref()?ref()->is_striding:is_striding; }

std::type_index Member::type() { return typeid( *this ); }

std::string Member::type_name() {

    if (type() == typeid(Struct)) return camel(name());

    if (ref()) return ref()->type_name();

    if (type() == typeid(glm::vec2)) return "vec2";

    if (type() == typeid(glm::vec3)) return "vec3";

    if (type() == typeid(glm::vec4)) return "vec4";

    if (type() == typeid(float)) return "float";

    if (type() == typeid(int)) return "int";

    if (type() == typeid(uint32_t)) return "uint";

    if (type() == typeid(char)) return "char";

    std::string type_ = type().name();

    if (isData()) return "datatype" + type_;

    return "unknown" + type_;

}

uint8_t Member::count() {

    // if (type() == typeid(Struct)) { int x = 0; for (auto m : members) { x += m.count()}; ); return x; }

    if (type() == typeid(glm::vec2)) return 2;

    if (type() == typeid(glm::vec3)) return 3;

    if (type() == typeid(glm::vec4)) return 4;

    return 1;

}

int Member::get_offset() {

    // for (auto owner : getTop()) {

    //     if (owner->isBuff()) { /* find in owner and do it stuff*/ }

    // }

    return 0;
}



Member* Member::copy() { return new Member(*this); }

bool Member::isData() { return false; }
Struct* Member::isStruct() { return nullptr; }
Buffer* Member::isBuff() { return nullptr; }


void Member::pre_change() {

    auto tops = getTop();
    for (auto x : tops)  
        x->pre_change();

}

void Member::post_change(std::vector<NewMember> added) {

    for (auto x : getTop()) 
        x->post_change(added);

}

std::string Member::next_name( std::string name ) {

    int count = 0;

    for (auto x : members) {

        auto name_ = x->name();

        size_t pos = name_.find("_");

        int i = 1;

        if (pos != std::string::npos) {

            name_ = x->name().substr(0, pos);

            i = std::stoi(x->name().substr(pos+1));

        }

        if (!strcmp(name.c_str(), name_.c_str())) {

            if ( i > count) count =  i;

            else count++;

        }

    }

    if (count) name += "_" + std::to_string(count) ;

    return name;

}
