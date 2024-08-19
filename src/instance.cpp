#include "instance.hpp"
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


bool Instance::exist(){

    if (offset == -1) return false;

    return true;

}

Instance::Instance(Buffer* buff, Member* m) : buff(buff) {
    
    if (buff) {
        
        offset = 0;
        auto minst = find(m);
        stl = minst.stl;
        offset = minst.offset;

    }else {
        
        stl.push_back(m);

        offset = 0;
            
    }

 }

Instance::Instance(Buffer* buff, uint32_t offset, std::vector<Member*> stl, int eq_id) : buff(buff), offset(offset), stl(stl), eq_id(eq_id) { }

Instance Instance::parent() {

    auto offset = this->offset;
    offset-=def()->footprint();
    auto stl_ = stl;
    stl_.resize(stl_.size()-1);

    return Instance(buff,offset,stl_);

}

Instance::Instance(std::string stl_name) {

    auto names = split(stl_name);

    if (!names.size()) return;

    for (auto x : Member::buffers) 

        if (x->name() == names[0]) {
        
            buff = x;
            names.erase(names.begin());
            break;    
        }

    if (!buff) { PLOGW << "no buffer " << names[0]; return; }

    Member* curr = buff;

    offset = 0;

    while (names.size()) {

        Member* found = nullptr;

        auto name = names[0];

        auto b1 = name.find("[");

        int eq = 0;

        if (b1 && name.back() == ']') {
            
            auto bracket = name.substr(b1+1,name.length()-4);

            for (int i = 0; i < bracket.length(); i++) 
                if (!std::isdigit(bracket[i])) 
                    return;

            
            eq = std::stoi(bracket);
            name = name.substr(0, b1);

            PLOGW << name;
            PLOGW << eq;

        }

        eq_id = 0;

        for (auto &m : curr->members) if (m->name() == name){ 
            if (eq < m->quantity()) {

                if (names.size() >1 ) offset += m->footprint() * eq;
                
                else eq_id = eq;

            }
            else{
                PLOGE << eq << " > " << m->quantity();
            }
            found = m;
            break;

        }else{

            offset += m->footprint_all();
        }

        if (!found) {

            PLOGE << "couldnt find: " << names[0];
            break;
            
        }

        names.erase(names.begin());
        stl.push_back(found);
        curr = found;

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

Instance Instance::operator[](std::string name) {

    auto offset = this->offset;

    Member* found = nullptr;

    for (auto &m : def()->members) {

        if (!(strcmp(m->name().c_str(),name.c_str()))) {

            found = m;

            break;

        }

        offset += m->footprint_all();

    }

    if (!found) PLOGE << "\"" << name << "\" does not exist in " << def()->name();
    auto stl_ = stl;
    stl_.push_back(found);
    return Instance{buff,offset,stl_};

}
Instance Instance::operator[](int id) {

    auto offset = this->offset;
    auto member = this->def();

    if (id >= def()->members.size()) {
    
        PLOGE << id << " > " << def()->members.size() << " in " << def()->name();
    
        return *this;
    
    }

    member = member->members[id];

    if (member->instances.size()) for (auto inst : member->instances) {

        auto c = inst->stl;
        c.resize(c.size()-1);

        if (c == stl) return *inst;
        
    }

    for (int i = 0 ; i < id; i ++ ){

        auto m = def()->members[i];

        offset += m->footprint_all();

    }


    auto stl_ = stl;
    stl_.push_back(member);

    return Instance{buff,offset,stl_};

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

Instance* Instance::this_() { 
    
    for (auto &x : def()->instances) if (x->stl == stl) return &(*x); 
    
     return this; 
     
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
