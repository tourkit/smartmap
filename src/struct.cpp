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

    size_v += addSize(c->size); 

}

void Struct::removeComp(std::string component){

    auto c = Component::exist(component.c_str()); if (!c) return;

    comps.erase(std::remove(comps.begin(), comps.end(), c), comps.end());

    size_v -= addSize(c->size);

}

size_t Struct::size() { return size_v; }

bool Struct::stride() { return is_striding; }

void Struct::stride(bool is_stride) {

    size_v = 0; 

    for (auto c: comps) size_v += addSize(c->size); 
    
}
int Struct::addSize(int size) {

    if (is_striding) size = ((int)(size/(float)16)+1)*16; //nextFactor(size,16);
    
    return size;

}