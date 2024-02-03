#include "struct.hpp"
#include "component.hpp"

Struct::Struct(std::string name, std::vector<std::string> components) :name(name) { addComp(components); }

void Struct::addComp(std::vector<std::string> components){

    size_t size = 0;

    for (auto c : components) {
        
        auto x = Component::id(c.c_str());

        comps.push_back(x);

        size += x->size;
        
    }

    this->size+=size;

}