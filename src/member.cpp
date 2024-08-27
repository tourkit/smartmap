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

    removing.insert(this);

    PLOGV << name() ;

    tops = getTop();

    for (auto t : tops)  
        t->stl.front().m->bkp();

    size_v = 0;

    deleteData();

    members.clear();

    update();

    tops.clear();

    removing.erase(this);

    return true;

}

void Member::update() { 

    if (!isData()){
        
        size_v = 0;

        for (auto &m : members) 
            size_v += m->footprint_all();

    }

    for (auto a : structs) 
        for (auto &m : a->members) 
            if (m == this) 
                a->update();

    if (isData()) return;

    PLOGV << name() << "[" << footprint_all() << "]";

    //// BUFFER PART

    if (buffering()) {

        if (footprint_all() > MAX_SIZE) { PLOGE << footprint_all() << " > MAX_SIZE"; }

        buffer_v.resize(footprint_all());

        memset( buffer_v.data(), 0, buffer_v.size() );

    }

    bool found = false;
    for (auto x : adding)
        if (x.m == this) {
            found = true;
            break;
        }

    
        for (auto t : tops)  {

            t->post_change(adding);

    // if (!found)
            t->stl.front().m->remap();

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
char* Member::to() { return rangedef.size()?rangedef.data()+(size_v*quantity_v):nullptr; }
char* Member::def() { return rangedef.size()?rangedef.data()+(size_v*quantity_v)*2:nullptr; }

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
        
        if (m->isData()) 
            
            delete m;
    }

}

std::set<std::shared_ptr<Instance>> Member::getTop(bool z) {

    std::set<std::shared_ptr<Instance>> owners, out;

    for (auto x : structs) 
        if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end())
            owners.insert( std::make_shared<Instance>(*x) );

    if (!owners.size()) { 
        if (!z && !buffering_v) 
            return {}; 
        return {std::make_shared<Instance>(*this)}; 
    }

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

    if (!out.size()) 
        return {std::make_shared<Instance>(*this)};

    return out;

}

void Member::remap(Member* src_buffer, Member* src_member, Member* this_member , int src_offset, int this_offset) {

    bool is_main = false;

    if (!src_buffer) {

        if (!bkp_v) {

            PLOGE << "no bkp for " << name();    

            return;
        
        }
        
        PLOGV << "remap " << bkp_v->name();

        src_buffer = bkp_v;

        is_main = true;
    }

    if (!src_member) 
        src_member = src_buffer;

    if (!src_buffer->buffer_v.size()) 
        return;

    if (!this_member) 
        this_member = this;

    for (int i = 0 ; i < ( src_member->quantity() < this_member->quantity() ? src_member->quantity() :  this_member->quantity() ); i ++) {

        int src_offset_ = src_offset + src_member->eq(i);

        for (auto src_member_ : src_member->members) {

            Member* found = nullptr;

            uint32_t thiseq = this_member->eq(i);

            int this_offset_ = this_offset + thiseq;

            for (auto this_member_ : this_member->members) {

                if (src_member_->copy_v == this_member_) { 
                    
                    found = this_member_; 
                    
                    break; 
                    
                } else 
                    
                    this_offset_ += this_member_->footprint_all();

            }


            if (!found ) {

                src_offset_ += src_member_->footprint_all();

                continue;
            }

            remap(src_buffer, src_member_, found, src_offset_, this_offset_);

            if (found->isData()) 

                for (int i = 0; i < found->quantity(); i++) {

                    int offset__ = found->footprint()*i;
                    int src_offset__ = src_offset_+offset__;
                    int this_offset__ = this_offset_+offset__;

                    PLOGV  << src_member->name() << "::" << src_member_->name() << "@" << src_offset__ << " -> "  << this_member->name() << "::" << found->name()  << "@" <<  this_offset__ << " : " << *(float*)&src_buffer->buffer_v[src_offset__];// << " - [" << src_member_->size() << "]";
    
                    memcpy(&buffer_v[this_offset__], &src_buffer->buffer_v[src_offset__],found->size());

                }

        
            src_offset_ += src_member_->footprint_all();

        }

    }

    if (is_main) {


        bkp_v->deleteData();

        delete bkp_v;

        bkp_v = nullptr;

    }

}
