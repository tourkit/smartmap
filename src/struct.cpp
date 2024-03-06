#include "struct.hpp"
#include "component.hpp"

Struct::~Struct() { pool.erase(this); }

Struct::Struct (const Struct& other) {

    name = other.name;
    comps = other.comps;
    size_v = other.size_v;

}

Struct::Struct(const char* name, std::vector<std::string> components) : name(name) { 
    
    pool.insert(this);

    for (auto c : components) addComp(c); 
    
}

void Struct::addComp(std::string component){

    auto c = Component::id(component.c_str());

    comps.push_back(c);

    auto size = c->size;
    if (c->members.size()>1) size = addSize(size);
    size_v += size; 

}

void Struct::removeComp(std::string component){

    auto c = Component::exist(component.c_str()); if (!c) return;

    comps.erase(std::remove(comps.begin(), comps.end(), c), comps.end());

    auto size = c->size;
    if (c->members.size()>1) size = addSize(size);
    size_v -= size; 


}

size_t Struct::size() { return size_v; }

bool Struct::stride() { return is_striding; }

void Struct::stride(bool is_striding) {

    this->is_striding = is_striding;

    size_v = 0; 

    for (auto c: comps) {
        auto size = c->size;
        if (c->members.size()>1) size = addSize(size);
        size_v += size; 
    }
    
}
int Struct::addSize(int size) {

    if (is_striding) size = nextFactor(size,16);
    
    return size;

}