#include "member.hpp"
#include "struct.hpp"
#include "instance.hpp"


#include "log.hpp"
#include "buffer.hpp"
#include "utils.hpp"
#include <cstdint>
#include <cstring>
#include <unordered_set>
#include <typeindex>
#include <cstdint>
#include <memory>

Member::~Member() {

    // std::stringstream ss; ss << std::hex << std::showbase << reinterpret_cast<void*>(this);

    if (!is_copy) 
        {PLOGV << "~" << name()/* << " ( &" + ss.str() + " )"*/;}

    // remove from other structs
    for (auto s : structs) 
        if (std::find(s->members.begin(), s->members.end(), this) != s->members.end()) 
            s->remove(*this);
    
    // remove from Member::structs
    if (!isData()) 
        structs.erase(this);

    // delete isData() a.k.a Data members
    for (auto x : members) 
        if (x->isData() )
            delete x;

    
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
    members(other.members) ,
    ref_v(other.ref_v),
    type_v(other.type_v), 
    size_v(other.size_v), 
    rangedef(other.rangedef) ,
    buffer_v(other.buffer_v),
    is_copy(true)

{

    for (auto &m : members) m = m->copy();

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

    for (auto x : structs) 
        if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end())
            owners.insert( x );

    if (!owners.size()) { 
        if (!z) 
            return {}; 
        return {this}; 
    }

    for (auto owner : owners) {

        auto top = owner->getTop(true);

        out.insert(top.begin(), top.end());

    }

    if (!out.size()) return {this};

    return out;

}


void Member::update() { 

    for (auto a : structs) 
        for (auto &m : a->members) 
            if (m == this) 
                a->update();

    size_v = 0;

    for (auto &m : members) size_v += m->footprint_all();

    PLOGV << name() << "[" << footprint_all() << "]";

    //// BUFFER PART

    if (!buffering()) return;

    if (footprint_all() > MAX_SIZE) { PLOGE << footprint_all() << " > MAX_SIZE"; }

    buffer_v.resize(footprint_all());

    memset( buffer_v.data(), 0, buffer_v.size() );

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

uint32_t Member::size() {     
    
    auto m = ref();
    
    if (m) return m->size();

    return size_v;

}

bool Member::clear() {

    PLOGV << name() ;

    auto tops = getTop();

    for (auto x : tops)  
        x->pre_change();

    size_v = 0;

    for (auto x : members) delete x;

    members.clear();

    update();

    for (auto x : tops) 
        x->post_change();

    return true;

}

uint32_t Member::footprint() { if (striding()) return nextFactor2(size(),16);  return size(); }

uint32_t Member::stride() { return (footprint()-size()); }

Member* Member::quantity(uint32_t quantity_v) {

    if (quantity_v == this->quantity_v) return this;

    PLOGV << name() << " = " << quantity_v;

    std::set<Member *> totops;

    auto tops = getTop();

    for (auto x : tops)  
        x->pre_change();

    auto old = this->quantity_v;

    this->quantity_v = quantity_v;

    update();

    for (auto x : tops)  
        x->post_change({{this, old, (int)quantity_v-old}});

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
    if (is_buffer) tops.insert(this);

    for (auto x : tops)  
        x->pre_change();

    members.push_back(m);

    size_v += members.back()->footprint_all();

    update();

    for (auto x : tops)  
        x->post_change({{m}});

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

    update();

    return true;
}

bool Member::remove(Member& m) {

    removing.insert(&m);

    PLOGV << name() << "[" << footprint_all() << "] remove " << m.name() << "[" << m.footprint_all() << "]";

    auto tops = getTop();

    for (auto x : tops)  
        x->pre_change();

    removeHard(m);

    // INSTANCEMGMT
    // remove corresponding isnt in M

    // for (auto it = m.instances.begin(); it != m.instances.end();){
    
    //     Member* owner;
    //     if (it->get()->stl.size()>1) 
    //         owner = it->get()->stl.back()-1;
    //     else 
    //         owner = it->get()->buff;
    
    //     if (owner == this) 
    //         it = m.instances.erase(it);
    //     else 
    //         it++;
    // }

    update();

    for (auto x : tops)  
        x->post_change();

    removing.erase(&m);

    return true;

}


uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i * footprint(); }

uint32_t Member::footprint_all() { return eq( quantity_v ); }

void Member::striding(bool is_striding){ this->striding_v = is_striding; update(); }

bool Member::striding() { return ref()?ref()->striding_v:striding_v; }

Type Member::type() { if (isData()) { for (auto x : members) return x->type(); } return type_v; }

std::string Member::type_name() {

    if (type().id == typeid(Struct)) return camel(name());

    if (ref()) return ref()->type_name();

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

Member* Member::copy() { return new Member(*this); }

bool Member::isData() { return type_v.id != typeid(Member); }

bool Member::isBuff() { return is_buffer; }

void Member::pre_change() {

    if (buffering()) {

        bkp = copy();

        PLOGV << "bkp " << name() ;

    }

}

char* Member::data() { 
    if (is_buffer) {
        char* ptr = &buffer_v[0];  
        return ptr;
    }
    return nullptr; 
}

bool Member::buffering() { return is_buffer; }

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

void Member::post_change(std::vector<NewMember> addeds) {

    Instance(*this).each([&](Instance& inst) {

        auto &mq = inst.stl.back();

        // instances update offset
        for (auto &x : mq.m->instances) {

            if (x->stl.size() == inst.stl.size() && x->stl.front().m == this) {

                bool diff = false;
                for (int i = 1; i < inst.stl.size(); i++) 
                    if (inst.stl[i].m != x->stl[i].m) {

                        diff = true;
                        break;

                    }

                if (!diff) 
                    x->offset = inst.offset+mq.m->footprint()*mq.eq; 

            }

        }

        for (auto added : addeds) 
            if (added.m == mq.m){
                for (int i = 0; i < added.q; i++)  {

                    inst.eq(added.eq+i);
                    inst.setDefault();

                }


            }

    });

    for (auto added : addeds) {

        // INSTANCEMGMT
        // Instance(this).each([&](Instance& inst) {

        //     bool found = false;
        //     if (inst.def() == added.m) 
        //         found = true;   
        //     else  
        //         if (inst.def()->ref()) 
        //             for (auto x : inst.stl) {
        //                 for (auto y : addeds) 
        //                     if ( x == y.m ) { 
        //                         found = true; 
        //                         break; 
        //                     }
                        
        //                 if (found) break;
        //             }    

        //     if (found) 
        //         inst.setDefault(); 

        // });

    }
    
    if (!bkp) return;

    PLOGV << "remap " << bkp->name();

    remap(*bkp);

    bkp->deleteData();

    delete bkp;

    bkp = nullptr;

    upload();

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


void Member::remap(Member& src_buffer, Member* src_member, Member* this_member , int src_offset, int this_offset) {

    if (!src_member) src_member = &src_buffer;

    if (!src_buffer.buffer_v.size()) return;

    if (!this_member) this_member = this;

    for (int i = 0 ; i < ( src_member->quantity() < this_member->quantity() ? src_member->quantity() :  this_member->quantity() ); i ++) {

        int src_offset_ = src_offset + src_member->eq(i);

        for (auto src_member_ : src_member->ref()?src_member->ref()->members:src_member->members) {

            Member* found = nullptr;

            uint32_t thiseq = this_member->eq(i);

            int this_offset_ = this_offset + thiseq;

            for (auto this_member_ : this_member->ref()?this_member->ref()->members:this_member->members) {

                if (

                    !strcmp(src_member_->name().c_str(), this_member_->name().c_str())

                    //&& src_member_->type().id == this_member_->type() // dont think thats relevent, fuxks

                ) { found = this_member_; break; }

                else this_offset_ += this_member_->footprint_all();

            }


            if (!found ) {

                src_offset_ += src_member_->footprint();

// #ifdef ROCH
               if (![src_member_](){ // COULD BE DEBUG MODE ONLY

                    for (auto x : removing) if (!strcmp(x->name().c_str(),src_member_->name().c_str())) return true;

                    return false;

                }())

                    PLOGE << "couldnt find " << src_member_->name() << " in " << src_buffer.name();
// #endif
                continue;
            }

            remap(src_buffer, src_member_, found, src_offset_, this_offset_);

            if (found->isData()) {

                PLOGV  << src_member->name() << "::" << src_member_->name() << "@" << src_offset_ << " -> "  << " " << this_member->name() << "::" << found->name()  << "@" <<  this_offset_<< " - " << src_member_->size() << " : " << (unsigned int)*(char*)&src_buffer.buffer_v[src_offset_] << " -> " << *(float*)&buffer_v[this_offset_];

                memcpy(&buffer_v[this_offset_], &src_buffer.buffer_v[src_offset_],found->size());

            }

            src_offset_ += src_member_->footprint_all();

        }

    }

}



/////////// DATA

void Member::type(Type value) {

    type_v = value;
#ifdef ROCH
    DEBUG_TYPE = value.name();
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
