#include "instance.hpp"
#include "struct.hpp"
#include "buffer.hpp"
#include "src/utils.hpp"
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

void Instance::setDefault(Member* toset, int offset) {

    if (!toset) toset = def();

    for (auto x : toset->members) {

        setDefault(x, offset);

        offset += x->footprint_all();

    }

        // PLOGD << "------------>set " << toset->name();
    if (toset->default_val_ptr) {
        // PLOGD << "------------>set " << toset->name() << " to default " << *(float*)toset->default_val_ptr << " @ " << offset;


        memcpy(data()+offset, toset->default_val_ptr, toset->size());

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

void Instance::remap(Instance& inst) {


    
}

Instance::Instance(const Instance& other) {

    buff = other.buff;
    offset = other.offset;
    stl = other.stl;
    eq_id = other.eq_id;

}

Instance::Instance(std::string stl_name) {

    auto names = split(stl_name);

    if (!names.size()) return;

    for (auto x : Member::buffers) if (x->name() == names[0]) buff = x;

    if (!buff) { PLOGE << "no member " << names[0]; }

    names.erase(names.begin());

    Member* curr = buff;

    for (auto name : names) {

        // for (auto x : curr->members) if (x->name() == x) m = x;

    }
 
}

void Instance::update() { for (auto &inst : def()->instances) for (auto r : inst.get()->remaps) r->update(); }

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
