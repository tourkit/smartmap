#include "struct.hpp"
#include "component.hpp"

Struct::~Struct() { pool.erase(this); }

Struct::Struct(const char* name, std::vector<std::string> components) : name(name) { 
    
    pool.insert(this);

    for (auto c : components) addComp(c); 
    
}

void Struct::addComp(std::string component){

    auto c = Component::id(component.c_str());

    comps.push_back(c);

    size_v += c->size;

}

size_t Struct::size() { return size_v; }
size_t Struct::stride() { return nextFactor(size_v-1,16)-size_v; }