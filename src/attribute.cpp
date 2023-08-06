#include "attribute.hpp"  

Attribute::Attribute(std::string name, float range_from, float range_to, float default_value)
: Attribute(name, nullptr, range_from, range_to, default_value) {  }

Attribute::Attribute(std::string name, float *data, float range_from, float range_to, float default_value)
: name(name), data(data), range_from(range_from), range_to(range_to), default_value(default_value) {  }

Attribute::Attribute(std::string name, std::vector<Attribute> childrens) 
: name(name) { for (auto c:childrens) this->childrens.push_back(c); }

Attribute* Attribute::child(std::string name) { 

    Attribute* attr = nullptr;

    for (auto c:childrens) {  if (c.name == name) { attr = &c; break; }  } 

    if (!attr) std::cout << "Child \"" << name << "\"" << " not found !" << std::endl; 
    
    return attr;

}

Attribute* Attribute::operator[] (uint16_t id) { return &childrens[id]; }