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

    // delete isData() a.k.a Data members
    for (auto x : members) 
        if (x->isData() )
            delete x;

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

Member::Member(const Member& other)
 :

    striding_v(other.striding_v) ,
    quantity_v( other.quantity_v ) ,
    name_v(other.name_v) ,
    members(other.members),
    type_v(other.type_v), 
    size_v(other.size_v), 
    rangedef(other.rangedef) ,
    buffer_v(other.buffer_v),
    copy_v((Member*)&other)

{

    for (auto &m : members) m = new Member(*m);

}

void Member::deleteData(){

    auto t_members = members;

    for (auto m : t_members) {

        
        if (m->isData()) 
            
            delete m;

        m->deleteData();
    }

}

std::set<std::shared_ptr<Instance>> Member::getTop(bool z) {

    std::set<std::shared_ptr<Instance>> owners, out;

    for (auto x : structs) 
        if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end())
            owners.insert( std::make_shared<Instance>(*x) );

    if (!owners.size()) { 
        if (!z && !is_buffer) 
            return {}; 
        return {std::make_shared<Instance>(*this)}; 
    }

    for (auto owner : owners) {

        auto top = owner.get()->stl.front().m->getTop(true);

        out.insert(top.begin(), top.end());

    }

    if (!out.size()) return {std::make_shared<Instance>(*this)};

    return out;

}


void Member::update(std::set<std::shared_ptr<Instance>>* tops, std::vector<MemberQ> addeds) { 

    for (auto a : structs) 
        for (auto &m : a->members) 
            if (m == this) 
                a->update(tops,addeds);

    if (isData()) return;

    size_v = 0;

    for (auto &m : members) size_v += m->footprint_all();

    PLOGV << name() << "[" << footprint_all() << "]";

    //// BUFFER PART

    if (!buffering()) return;

    if (footprint_all() > MAX_SIZE) { PLOGE << footprint_all() << " > MAX_SIZE"; }

    buffer_v.resize(footprint_all());

    memset( buffer_v.data(), 0, buffer_v.size() );

    if (!tops) return;

    for (auto t : *tops)  {

        t->post_change(addeds);

        t->stl.front().m->remap();

        t->stl.front().m->upload();    
        
    }
 }

void Member::name(std::string name_v) { this->name_v = next_name(name_v); }

std::string Member::name() { 
    if (name_v.length()) 
        return name_v; 
    
    return "parentName" ; 
}

uint32_t Member::size() {  

    return size_v;

}

bool Member::clear() {

    PLOGV << name() ;

    auto tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    size_v = 0;

    for (auto x : members) delete x;

    members.clear();

    update(&tops);

    return true;

}

uint32_t Member::footprint() { if (striding()) return nextFactor2(size(),16);  return size(); }

uint32_t Member::stride() { return (footprint()-size()); }

Member* Member::quantity(uint32_t quantity_v) {

    if (quantity_v == this->quantity_v) return this;

    PLOGV << name() << " = " << quantity_v;

    auto tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    auto old = this->quantity_v;

    this->quantity_v = quantity_v;

    update(&tops,{{this, old, (int)quantity_v-old}});

    return this;

}

void Member::upload() { 

    
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

    auto tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    members.push_back(m);

    size_v += members.back()->footprint_all();

    update(&tops, {{m}});

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
            (int&)*m->def() =  def;

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

    return *this;

}

bool Member::removeHard(Member& m) {

    auto it = std::find( members.begin(), members.end(), &m );

    if (it == members.end()) { PLOGV << "no find "<< m.name(); return false; }

    size_v -= members.back()->footprint_all();

    members.erase(it);

    return true;
}

bool Member::remove(Member& m) {

    removing.insert(&m);

    PLOGV << name() << "[" << footprint_all() << "] remove " << m.name() << "[" << m.footprint_all() << "]";

    auto tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    removeHard(m);

    update(&tops);

    removing.erase(&m);

    return true;

}


uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i * footprint(); }

uint32_t Member::footprint_all() { return eq( quantity_v ); }

void Member::striding(bool is_striding){ this->striding_v = is_striding; update(); }

bool Member::striding() { return striding_v; }

Type Member::type() { if (isData()) { for (auto x : members) return x->type(); } return type_v; }

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


bool Member::isData() { return type_v.id != typeid(Member); }

char* Member::data() { 
    if (is_buffer) {
        char* ptr = &buffer_v[0];  
        return ptr;
    }
    return nullptr; 
}

bool Member::buffering() { return is_buffer; }

void Member::bkp() {

    if (!is_buffer) return;

    bkp_v = new Member(*this);

    PLOGV << "bkp " << name() ;

}

void Member::buffering(bool value) { 
    
    is_buffer  = value;

    if (is_buffer) {

        buffer_v.reserve(MAX_SIZE);

        memset(buffer_v.data(),0,MAX_SIZE);


    }else{

        buffer_v.reserve(0);

    }

    update();

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


void Member::remap(Member* src_buffer, Member* src_member, Member* this_member , int src_offset, int this_offset) {

    auto first = false;
    if (!src_buffer) {

        if (!bkp_v) {

            PLOGE << "no bkp";    
            return;
        }
        
        PLOGV << "remap " << bkp_v->name();

        src_buffer = bkp_v;
        first = true;
    }

    if (!src_member) 
        src_member = src_buffer;


    if (!src_buffer->buffer_v.size()) return;

    if (!this_member) this_member = this;

    for (int i = 0 ; i < ( src_member->quantity() < this_member->quantity() ? src_member->quantity() :  this_member->quantity() ); i ++) {

        int src_offset_ = src_offset + src_member->eq(i);

        for (auto src_member_ : src_member->members) {

            Member* found = nullptr;

            uint32_t thiseq = this_member->eq(i);

            int this_offset_ = this_offset + thiseq;

            for (auto this_member_ : this_member->members) {

                if (

                    !strcmp(src_member_->name().c_str(), this_member_->name().c_str())

                ) { found = this_member_; break; }

                else this_offset_ += this_member_->footprint_all();

            }


            if (!found ) {

                src_offset_ += src_member_->footprint_all();

#ifdef ROCH
               if (![src_member_](){ // COULD BE DEBUG MODE ONLY

                    for (auto x : removing) if (!strcmp(x->name().c_str(),src_member_->name().c_str())) return true;

                    return false;

                }())

                    { PLOGE << "couldnt find " << src_member_->name() << " in " << src_buffer->name(); }
#endif
                continue;
            }

            remap(src_buffer, src_member_, found, src_offset_, this_offset_);

            if (found->isData()) 

                for (int i = 0; i < found->quantity(); i++) {

                    int offset__ = found->footprint()*i;
                    int src_offset__ = src_offset_+offset__;
                    int this_offset__ = this_offset_+offset__;

                    PLOGV  << src_member->name() << "::" << src_member_->name() << "@" << src_offset__ << " -> "  << " " << this_member->name() << "::" << found->name()  << "@" <<  this_offset__<< " - " << src_member_->size() << " : " << (unsigned int)*(char*)&src_buffer->buffer_v[src_offset__] << " -> " << *(float*)&buffer_v[this_offset__];
    
                    memcpy(&buffer_v[this_offset__], &src_buffer->buffer_v[src_offset__],found->size());

                }

        
            src_offset_ += src_member_->footprint_all();

        }

    }

    if (first) {


        bkp_v->deleteData();

        delete bkp_v;

        bkp_v = nullptr;

    }

}



/////////// DATA

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

    else if (value.id == typeid(uint8_t)) to = 255;

    else if (value.id == typeid(char)) to = -1;
    
    range(0,to,0);


}

char* Member::from() { return rangedef.size()?rangedef.data():nullptr; }
char* Member::to() { return rangedef.size()?rangedef.data()+(size_v*quantity_v):nullptr; }
char* Member::def() { return rangedef.size()?rangedef.data()+(size_v*quantity_v)*2:nullptr; }
