#include "instance.hpp"
#include "member.hpp"
#include "struct.hpp"
#include "buffer.hpp"
#include "src/utils.hpp"
#include <cctype>
#include <cstdint>
#include <format>
#include <string>
#include <vector>


Remap::Remap(Instance* src , Instance* dst, int quantity) : src(src) ,dst(dst) , quantity(quantity) { }

void Remap::update() {

    int size = src->def()->size();

    if (dst->def()->size()<size) size = dst->def()->size();

    memcpy(dst->data(), src->data(), quantity * size);

}


Instance::Instance(Member* m, int eq_id, uint32_t offset) 

    : offset(offset), eq_id(eq_id) { 
        
        stl.push_back({m});
}

Instance Instance::parent() {

    Instance inst(*this);

    inst.offset-=def()->footprint();

    inst.stl.resize(inst.stl.size()-1);

    return inst;

}

void Instance::calcOffset() {

    offset = 0;
        
    for (int i = 0; i < stl.size()-1; i++) {

        auto m = stl[i];

        offset += m.m->footprint()*m.eq;

    }

}


std::pair<std::string,int> nameQ(std::string name) { 
    
    auto inbracket = name.find("[");

    int eq = 0;

    if (inbracket && name.back() == ']') {
        
        auto bracket = name.substr(inbracket+1,name.length()-4);

        bool all_digits = true;
        for (int i = 0; i < bracket.length(); i++) 
            if (!std::isdigit(bracket[i])) {
                all_digits = false;  
                break;
            }
        
        if (all_digits) 
            eq = std::stoi(bracket);
        else {PLOGE << "WEIRDASHIZE";}

        name = name.substr(0, inbracket);

    }
    
    return {name,eq}; 
        
}

Instance::Instance(std::string stl_name) {

    auto names = split(stl_name);

    if (!names.size()) return;

    offset = 0;

    for (auto x : Member::structs) {

        auto name = nameQ(names[0]);

        if (x->name() == name.first) {
        
            stl.push_back({x});
            offset += x->footprint()*name.second;
            names.erase(names.begin());
            break;    

        }

    }

    MemberQ &curr = *stl.begin();

    while (names.size()) {

        Member* found = nullptr;

        auto name = nameQ(names[0]);

        curr.eq = 0;

        for (auto &m : curr.m->members) 
        
            if (m->name() == name.first){ 

                if (name.second < m->quantity()) {

                    if (names.size() >1 ) offset += m->footprint() * name.second;
                    
                    else curr.eq = name.second;

                } else { PLOGE << name.second << " > " << m->quantity(); }

                found = m;

                break;

            }else

                offset += m->footprint_all();
            

        if (!found) {

            PLOGE << "couldnt find: " << names[0];
            break;
            
        }

        names.erase(names.begin());

        stl.push_back({found,name.second});

        curr = stl.back();

    }
 
}

Instance Instance::find(Member* x) {

    Instance inst;

    each([&](Instance inst_){

        if (x == inst_.def())
            inst = inst_;

    });

    if (!inst.stl.size()) PLOGE << "\"" << x->name() << "\" does not exist in " << def()->name();

    return inst;

}

Instance Instance::child(std::string name) {

    auto m = def();

    for (int i = 0; i < m->members.size(); i++) 
        if (!(strcmp(m->members[i]->name().c_str(),name.c_str()))) return child(i);

    PLOGE << "\"" << name << "\" does not exist in " << def()->name();

    return *this;

}

Instance Instance::child(int id) {

    auto m = this->def();

    if (id >= m->members.size()) {
    
        PLOGE << id << " > " << m->members.size() << " in " << m->name();
    
        return *this;
    
    }

    Instance inst(*this);

    for (int i = 0 ; i < id; i ++ )
        inst.offset += m->members[i]->footprint_all();
    
    inst.stl.push_back(m->members[id]);

    return inst;

}

Instance Instance::eq(int id) {

    if ( id >= def()->quantity()) {
        
        PLOGE<<"Not that many quantity exist (" << id << " < " << def()->quantity() << ")" ;
        
        return *this;
        
    }

    uint32_t new_offset = offset + def()->footprint() * (id-eq_id);

    return Instance{buff, new_offset ,stl, id};

}

void Instance::setDefault(Member* m, int offset) {

    if (!m) m = def();

    for (auto x : m->members) {

        setDefault(x, offset);

        offset += x->footprint_all();

    }

        // PLOGD << "------------>set " << m->name();
    if (m->default_val_ptr) {
        // PLOGD << "------------>set " << m->name() << " to default " << *(float*)m->default_val_ptr << " @ " << offset;

        for (int i = stl.size()-1; i>=0; i--) { // if parent q>1

            auto x = stl[i];

            if (x->quantity()>1) 
                for (int todo = 0 ; todo < x->quantity(); todo++) 
                    memcpy(data()+(offset+x->size()*todo), m->default_val_ptr, m->size());


        }

        memcpy(data()+offset, m->default_val_ptr, m->size());

    }

}

Instance Instance::push(void* ptr, size_t size) {

    PLOGV << "new " << def()->name() << " in " << buff->name() ;

    def()->quantity(def()->quantity()+1);

    auto inst = eq(def()->quantity()-1);

    if (!ptr) { inst.setDefault(); }

    else {

        if (!size) size = def()->size();

        inst.set(ptr,size);

    }

    return inst;

}

void Instance::remap(std::string stl_name) { 
    Instance inst(stl_name); 
    remap(inst); 
}

void Instance::remap(Instance& inst) {

    auto this_ = &this->track();

    this_->remaps.push_back(new Remap(this_, &inst.track())); // waiwai .. check ~Inst :) 

}

Instance::Instance(const Instance& other) {

    buff = other.buff;
    offset = other.offset;
    stl = other.stl;
    eq_id = other.eq_id;

}

void Instance::print() {

    std::string out;

    each([&](Instance inst){

        auto m = inst.def();
        if (m->isData()) {

            out += inst.stl_name() + ": ";

            if (m->type() == typeid(int)) out+= std::to_string((int)*inst.data());
            else if (m->type() == typeid(float)) {
                std::string float_ = std::to_string((float)*inst.data());
                while(float_.back() == '0') float_.resize(float_.length()-1);
                if(float_.back() == '.') float_.resize(float_.length()-1);
                out+= float_;
            }
            else return;
            out += ", ";

        }

    });

    if (out.length()) out = out.substr(0,out.length()-2);

    PLOGW << out;

}

void Instance::update() { 
    
    for (auto &inst : def()->instances) 
    
        for (auto r : inst.get()->remaps) 
        
            r->update(); 
    
}

std::string Instance::stl_name(std::string separator) {

    std::string str;

    for (auto x : stl)  str += x->name() + separator;

    if (str.length()) str.resize(str.length()-separator.length());

    else str = buff->name();

    return str;

}


Instance& Instance::track() {

    auto m = def();

    for (auto inst : m->instances) {

        if (inst->stl == stl) return *inst;
        
    }

    PLOGV << (buff?buff->name()+"::":"") << stl_name();

    def()->instances.emplace_back(std::make_shared<Instance>(buff, offset, stl));

    return *def()->instances.back().get();

}



char* Instance::data() { return buff->data.data()+offset; }

uint32_t Instance::size() { return def()->footprint_all(); }

Instance& Instance::set(void* ptr, size_t size) {

    memcpy(data(), ptr, size);

    update();

    return *this;

}



Member* Instance::buff() { 
    
    if (stl.size() && stl[0]->buffering()) 
    
        return stl.begin(); 
    
    return buff; 
    
}



Member* Instance::def() { 
    
    if (stl.size()) 
    
        return stl.back(); 
    
    return buff; 
    
}



void Instance::each(std::function<void(Instance&)> cb) { 

    auto x = def();

    int offset = 0;

    for (auto m : x->ref()?x->ref()->members:x->members) {

        Instance inst(*this);

        inst.stl.push_back(m);

        inst.offset += offset;

        inst.each(cb);

        cb(inst);

        offset += m->footprint_all();

    }

}
