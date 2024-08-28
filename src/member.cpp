#include "member.hpp"
#include "struct.hpp"
#include "instance.hpp"


#include "buffer.hpp"
#include "utils.hpp"
#include <cstdint>
#include <cstring>
#include <cstdint>

Member::~Member() {

    // remove from other structs
    for (auto s : structs) 
        if (std::find(s->members.begin(), s->members.end(), this) != s->members.end()) {
            s->remove(*this);  
}
    // remove from Member::structs
    if (!isData()) 
        structs.erase(this);

    deleteData(false);

    if (copy_v) return;

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);
    PLOGV << "~" << name()/* << " ( &" + ss.str() + " )"*/;
    
}

Member::Member(std::string name) {

    this->name(name);

    PLOGV << "#" << this->name();// << " ( &" + ss.str() + " )" ;

    type<Member>();

    structs.insert(this);

}

Member::Member(std::string name, Type type) {

    this->name(name);
    this->type(type);

}

Member::Member(Member& other) :
 
    striding_v(other.striding_v) ,
    quantity_v( other.quantity_v ) ,
    name_v(other.name_v) , // no need
    members(other.members),
    type_v(other.type_v), 
    size_v(other.size_v), 
    rangedef(other.rangedef) ,
    buffering_v(other.buffering_v),
    buffer_v(other.buffer_v),
#ifdef ROCH
    _TYPE_(other._TYPE_),
#endif
    copy_v(&other)

{

    for (auto &m : members) 
        m = new Member(*m);

}


void Member::name(std::string name_v) { this->name_v = next_name(name_v); }

std::string Member::name() { 
    if (name_v.length()) 
        return name_v; 
    
    return "parentName" ; 
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

std::string Member::type_name() {

    if (type().id == typeid(Struct)) return camel(name());

    if (type().id == typeid(float)) {
        
        if (quantity_v == 1) return "float";
        else if (quantity_v == 2) return "vec2";
        else if (quantity_v == 3) return "vec3";
        else if (quantity_v == 4) return "vec4";
        
    }

    if (type().id == typeid(int)) return "int";

    if (type().id == typeid(uint32_t)) return "uint";

    if (type().id == typeid(char)) return "char";

    std::string type_ = type().name();

    if (isData()) return "datatype" + type_;

    return "unknown" + type_;

}

void Member::bkp() {

    if (!buffering_v) return;

    bkp_v = new Member(*this);

    PLOGV << "bkp " << name() ;

}



void Member::type(Type value) {

    type_v = value;
#ifdef ROCH
    _TYPE_ = value.name();
#endif

    if (!isData()) {
        
        size_v = 0;

        return;
    
    }

    size_v = value.size();

    rangedef.resize(size_v*3);

    memset(rangedef.data(),0,rangedef.size());

    float to = 1;

    if (value.id == typeid(uint32_t)) to = 65535;
    
    // else if (value.id == typeid(int)) to = 65535;

    else if (value.id == typeid(uint8_t)) to = 255;

    else if (value.id == typeid(char)) to = -1;
    
    range(0,to,0);


}


void Member::buffering(bool value) { 
    
    buffering_v  = value;

    if (buffering_v) {

        buffer_v.reserve(MAX_SIZE);

        memset(buffer_v.data(),0,MAX_SIZE);


    }else{

        buffer_v.reserve(0);

    }

    update();

}


Member& Member::striding(bool is_striding){ 

    tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();
    
    this->striding_v = is_striding; 

    update();

    tops.clear();

    return *this;
    
}

Member& Member::quantity(uint32_t quantity_v) {

    if (quantity_v == this->quantity_v) return *this;

    PLOGV << name() << " = " << quantity_v;

    tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    auto old = this->quantity_v;

    this->quantity_v = quantity_v;

    auto mq = adding.emplace_back(MemberQ{this, old, (int)quantity_v-old});
    update();
    REMOVE<MemberQ>(adding,mq);

    tops.clear();

    return *this;

}


void Member::add(Member* m) {

    PLOGV << name() << "[" << footprint_all() << "] add " << m->name() << "[" << m->footprint_all() << "]";

    while (true) {

        bool found = false;

        for (auto x : members) 

            if (!strcmp( x->name().c_str(), m->name().c_str() )) {

                found = true;

                PLOGW << m->name() << " already exist !";

                m->name( next_name(m->name()) );

                break ;

            }

        if (!found) break;

    }

    tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    members.push_back(m);

    size_v += members.back()->footprint_all();

    auto mq = adding.emplace_back(MemberQ{m});
    update();
    REMOVE<MemberQ>(adding,mq);

    tops.clear();

}



Member& Member::range(float from, float to, float def) {

    auto m = this;
    if (members.size()) 
        m = members.back();

    for (int i = 0; i < quantity_v; i++) {

        if (m->type().id == typeid(float)) {

            (float&)*m->from() = from;
            (float&)*m->to() = to;
            (float&)*m->def() = def;

        }else if (m->type().id == typeid(uint32_t)) {

            (uint32_t&)*m->from() =  from;
            (uint32_t&)*m->to() =  to;
            (uint32_t&)*m->def() =  def;

        }else if (m->type().id == typeid(int)) {

            (int&)*m->from() =  from;
            (int&)*m->to() =  to;
            (int&)*m->def() =  (int)def;

        }else if (m->type().id == typeid(char)) {

            (char&)*m->from() =  from;
            (char&)*m->to() =  to;
            (char&)*m->def() =  def;

        }else if (m->type().id == typeid(uint8_t)) {

            (uint8_t&)*m->from() =  from;
            (uint8_t&)*m->to() =  to;
            (uint8_t&)*m->def() =  def;

        }

    }

    for (auto s : structs) {

        for (auto m : s->members) 
            if (m == this) 
                for (auto top : m->getTop())  
                    top->post_change({{m,(int)(m->quantity()-1)}});

    }


    return *this;

}

bool Member::remove(Member& m) {

    removing.insert(&m);

    PLOGV << name() << "[" << footprint_all() << "] remove " << m.name() << "[" << m.footprint_all() << "]";

    tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    removeHard(m);

    update();

    removing.erase(&m);

    tops.clear();

    return true;

}

bool Member::removeHard(Member& m) {

    auto it = std::find( members.begin(), members.end(), &m );

    if (it == members.end()) { PLOGV << "no find "<< m.name(); return false; }

    size_v -= members.back()->footprint_all();

    members.erase(it);

    return true;
}


bool Member::clear() {

    PLOGV << name() ;

    tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    size_v = 0;

    deleteData(false);

    members.clear();

    // update();

    tops.clear();

    return true;

}

void Member::simpleupdate() { 




}

void remap2(Instance this_, Instance bkp) {

    auto m = bkp.stl.back().m;
    
    // each member
    for (auto bkp_m : m->members) {

        // look for in this_
        Member* found = nullptr;
        for (auto _this_m : this_.stl.back().m->members) 
            if (bkp_m->copy_v == _this_m) {
                found = _this_m;
                break;
            }

        if (!found) 
            continue;

        auto bkp_ = Instance(bkp)[bkp_m];
        auto this__ = Instance(this_)[found];

        int q = found->quantity()<bkp_m->quantity()?found->quantity():bkp_m->quantity();
        
        // each eq
        for (int i = 0; i < q; i++) 
            remap2(this__.eq(i), bkp_.eq(i));

    }

    if (m->isData())
        memcpy(this_.data(), bkp.data(),m->size());

    // PLOGV << bkp.stl_name() << " @ " << bkp.offset << " -> " << this_.offset;
    
}


void Member::update() { 


    size_v = 0;

    if (isData())
        size_v = type_v.size();
    else
        for (auto &m : members) 
            size_v += m->footprint_all();

    for (auto a : structs) 
        for (auto &m : a->members) 
            if (m == this) 
                a->update();

    // if (isData()) {

    //     bool is_adding = false;

    //     for (auto x : adding) 
    //         if (x.m == this) {

    //             is_adding = true;

    //             break;

    //         }

    //     if (!is_adding) 
    //         return;

    // }

    PLOGV << name() << "[" << footprint_all() << "]";

    //// BUFFER PART

    if (buffering()) {

        if (footprint_all() > MAX_SIZE) { PLOGE << footprint_all() << " > MAX_SIZE"; }

        buffer_v.resize(footprint_all());

        memset( buffer_v.data(), 0, buffer_v.size() );

    }
    
    for (auto t : tops)  {

        t->post_change(adding);

        remap2(*t,Instance(*t->stl.front().m->bkp_v));

        auto bkp_v = t->stl.front().m->bkp_v;

        if (bkp_v) {

            bkp_v->deleteData();

            delete bkp_v;

            bkp_v = nullptr;

        }

        t->stl.front().m->upload();    
        
    }

 }

 void Member::upload() { 

    
}

char* Member::data() { 

    if (buffering_v) {

        char* ptr = &buffer_v[0];  
        return ptr;
        
    }

    return nullptr; 

}

char* Member::from() { return rangedef.size()?rangedef.data():nullptr; }
char* Member::to() { return rangedef.size()?rangedef.data()+(size_v):nullptr; }
char* Member::def() { 
    
    if (rangedef.size())
        return rangedef.data()+(size_v*2);

    return nullptr; 

}

bool Member::buffering() { return buffering_v; }

bool Member::striding() { return striding_v; }

bool Member::isData() { return type_v.id != typeid(Member); }

uint32_t Member::size() {  return size_v; }

uint32_t Member::footprint() { if (striding()) return nextFactor2(size(),16);  return size(); }

uint32_t Member::stride() { return (footprint()-size()); }

uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i * footprint(); }

uint32_t Member::footprint_all() { return eq( quantity_v ); }

Type Member::type() { if (isData()) { for (auto x : members) return x->type(); } return type_v; }



void Member::deleteData(bool recurse){

    auto t_members = members;

    for (auto m : t_members) {

        if (recurse) m->deleteData();
        
        if (m->isData()) {

            removeHard(*m);
            
            delete m;}
            
    }

}

std::set<std::shared_ptr<Instance>> Member::getTop(bool z) {

    std::set<std::shared_ptr<Instance>> owners, out;

    for (auto x : structs) 
        if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end())
            owners.insert( std::make_shared<Instance>(*x) );



    for (auto owner : owners) {

        auto tops = owner.get()->stl.front().m->getTop(true);

        bool found = false;

        for (auto top : tops) {

            for (auto x : out) {

                if (x->stl.front().m == top->stl.front().m) {
                    found = true;
                    break;
                }

            }
                
            if (found) break;

        }

        if (!found) out.insert(tops.begin(), tops.end());

    }

    if (!out.size() && buffering()) 
        return {std::make_shared<Instance>(*this)};

    return out;

}