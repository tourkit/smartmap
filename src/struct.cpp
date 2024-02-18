#include "struct.hpp"
#include "component.hpp"

Struct::Struct(const char* name, std::vector<std::string> components) : name(name) { for (auto c : components) addComp(c); }

void Struct::addComp(std::string component){

    auto x = Component::id(component.c_str());

    comps.push_back(x);

    size += x->size;

}