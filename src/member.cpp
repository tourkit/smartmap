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

    if (!is_copy) 
        {PLOGV << "~" << name()/* << " ( &" + ss.str() + " )"*/;}

    removing.insert(this);

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


    removing.erase(this);
    
}

Member::Member(std::string name) {

    this->name(name);

    PLOGV << "#" << this->name();// << " ( &" + ss.str() + " )" ;

    type<Member>();

    structs.insert(this);

}

Member::Member(std::string name, TypeIndex type) {

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

    for (auto x : structs) if (std::find( x->members.begin(), x->members.end(), this ) != x->members.end()) owners.insert( x );

    if (!owners.size()) { if (!z) return {}; return {this}; }

    for (auto owner : owners) {

        auto top = owner->getTop(true);

        out.insert(top.begin(), top.end());

    }

    return out;

}

bool Member::each(std::function<bool(Member*,int)> cb, int offset) {

    for (auto m : members) {

        if (!m->each(cb, offset)) return false;

        if (!cb(m,offset)) return false;

        offset+=m->footprint_all();
        
    }

    return true;

}

bool Member::each(std::function<bool(Member*)> cb) {

    return each([cb](Member* m, int offset) { return cb(m); }, 0);

}

void Member::update() { 

    PLOGV << name();

    for (auto a : structs) 
        for (auto &m : a->members) 
            if (m == this) 
                a->update();

    size_v = 0;

    for (auto &m : members) size_v += m->footprint_all();

    //// BUFFER PART

    if (!buffering()) return;

    if (buffer_v.size() > MAX_SIZE) { PLOGE << "buffer_v.size() > MAX_SIZE"; }

    buffer_v.resize(footprint_all());

    memset( buffer_v.data(), 0, buffer_v.size() );

    for (auto s : structs) 
        for (auto &inst : s->instances) 
            if (inst->stl.size() && inst->stl.front().m == this)
                inst->calcOffset();

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

    PLOGV << name() << " = " << quantity_v;

    auto tops = getTop();

    for (auto x : tops)  
        pre_change();

    auto old = this->quantity_v;

    this->quantity_v = quantity_v;

    update();

    for (auto x : tops)  
        post_change({{this, old, (int)quantity_v-old}});

    return this;

}

void Member::upload() { 

    
}


void Member::add(Member* m) {

    PLOGV << name() << " add " << m->name();

    while (true) {

        bool found = false;

        for (auto x : members) {

            if (!strcmp( x->name().c_str(), m->name().c_str() )) {

                found = true;

                 PLOGW << m->name() << " already exist !";

                m->name( next_name(m->name()) );

                break ;

            }

        }

        if (!found) break;

    }

    auto tops = getTop();

    for (auto x : tops)  
        pre_change();

    members.push_back(m);

    size_v += members.back()->footprint_all();

    update();

    for (auto x : tops)  
        post_change({{m}});

}

Member& Member::range(float from, float to, float def) {

    auto m = members.back();

    for (int i = 0; i < quantity_v; i++) {

        if (m->type() == typeid(float)) {

            *(m->from()+sizeof(m->type_v)*i) = from;
            *(m->to()+sizeof(m->type_v)*i) = to;
            *(m->def()+sizeof(m->type_v)*i) = def;

        }else if (m->type() == typeid(uint32_t)) {

            *(m->from()+sizeof(m->type_v)*i) = (uint32_t) from;
            *(m->to()+sizeof(m->type_v)*i) = (uint32_t) to;
            *(m->def()+sizeof(m->type_v)*i) = (uint32_t) def;

        }else if (m->type() == typeid(int)) {

            *(m->from()+sizeof(m->type_v)*i) = (int) from;
            *(m->to()+sizeof(m->type_v)*i) = (int) to;
            *(m->def()+sizeof(m->type_v)*i) = (int) def;

        }else if (m->type() == typeid(char)) {

            *(m->from()+sizeof(m->type_v)*i) = (char) from;
            *(m->to()+sizeof(m->type_v)*i) = (char) to;
            *(m->def()+sizeof(m->type_v)*i) = (char) def;

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

    PLOGV << name() << " remove " << m.name();

    auto tops = getTop();

    for (auto x : tops)  
        pre_change();

    removeHard(m);

    // INSTANCEMGMT
    // std::vector<Instance*> delete_list;

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
        post_change();

    removing.erase(&m);

    return true;

}


uint32_t Member::quantity() { return quantity_v; }

uint32_t Member::eq(int i) { return i * footprint(); }

uint32_t Member::footprint_all() { return eq( quantity_v ); }

void Member::striding(bool is_striding){ this->striding_v = is_striding; update(); }

bool Member::striding() { return ref()?ref()->striding_v:striding_v; }

TypeIndex Member::type() { if (isData()) { for (auto x : members) return x->type(); } return typeid( *this ); }

std::string Member::type_name() {

    if (type() == typeid(Struct)) return camel(name());

    if (ref()) return ref()->type_name();

    if (type() == typeid(float)) {
        
        if (quantity_v == 1) return "float";
        else if (quantity_v == 2) return "vec2";
        else if (quantity_v == 3) return "vec3";
        else if (quantity_v == 4) return "vec4";
        
    }

    if (type() == typeid(int)) return "int";

    if (type() == typeid(uint32_t)) return "uint";

    if (type() == typeid(char)) return "char";

    std::string type_ = type().name();

    if (isData()) return "datatype" + type_;

    return "unknown" + type_;

}

Member* Member::copy() { return new Member(*this); }

bool Member::isData() { return type_v != typeid(Member); }

bool Member::isBuff() { return is_buffer; }

void Member::pre_change() {

    if (buffering()) {

        bkp = copy();

        PLOGV << "bkp " << name() ;

    }

}

char* Member::data() { if (is_buffer) return buffer_v.data(); return nullptr; }

bool Member::buffering() { return is_buffer; }

void Member::buffering(bool value) { 
    
    is_buffer  = value;

    if (is_buffer) {

        buffer_v.reserve(MAX_SIZE);

        memset(buffer_v.data(),0,buffer_v.size());


    }else{

        buffer_v.reserve(0);

    }

}

void Member::post_change(std::vector<NewMember> addeds) {

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

            auto thiseq = this_member->eq(i);

            int this_offset_ = this_offset + thiseq;

            for (auto this_member_ : this_member->ref()?this_member->ref()->members:this_member->members) {

                if (

                    !strcmp(src_member_->name().c_str(), this_member_->name().c_str())

                    //&& src_member_->type() == this_member_->type() // dont think thats relevent, fuxks

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

                // PLOGV  << src_member->name() << "::" << src_member_->name() << "@" << src_offset_ << " -> "  << " " << this_member->name() << "::" << found->name()  << "@" <<  this_offset_<< " - " << src_member_->size() << " : " << (unsigned int)*(char*)&src_buffer.data[src_offset_] << " -> " << *(float*)&data[this_offset_];

                memcpy(&buffer_v[this_offset_], &src_buffer.buffer_v[src_offset_],found->size());

            }

            src_offset_ += src_member_->footprint_all();

        }

    }

}



/////////// DATA

void Member::type(TypeIndex value) {

    type_v = value;
#ifdef ROCH
    DEBUG_TYPE = type_v.pretty_name();
#endif

    if (isData()) size_v = sizeof(value);

    else size_v = 0;

    rangedef.resize(size_v*3);

    if (!isData()) return;
    
    memset(rangedef.data(),0,rangedef.size());

    if (value == typeid(float))   *(float*)to() = 1.0f;

    else if (value == typeid(uint32_t))   *(uint32_t*)to() = 65535;

    else if (value == typeid(uint8_t))   *(uint32_t*)to() = 255;

    else if (value == typeid(char))   *(char*)to() = -1;

}

char* Member::from() { return rangedef.data(); }
char* Member::to() { return rangedef.data()+(size_v*quantity_v); }
char* Member::def() { return rangedef.data()+(size_v*quantity_v)*2; }
