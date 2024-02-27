#include "struct.hpp"
#include "component.hpp"

Struct::~Struct() { pool.erase(this); }

Struct::Struct(const char* name, std::vector<std::string> components) : name(name) { 
    
    pool.insert(this);

    for (auto c : components) addComp(c); 
    
}

void Struct::addComp(std::string component){

    auto x = Component::id(component.c_str());

    comps.push_back(x);

    size += x->size;

}