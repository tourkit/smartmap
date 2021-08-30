#include "attribute.hpp"  


#include <unordered_set>
#include <string>

Attribute::~Attribute() { 
  
  for (auto* attr:pool) { attr->links.erase(this); }  
  
  pool.erase(this); 
  
}



Attribute::Attribute(float v) : Attribute() { 
  
  set(v); 

}

Attribute::Attribute(int v) : Attribute() { 
  
  set((float)v); 
  
}

Attribute::Attribute(std::vector<float>* data = nullptr) {

  pool.insert(this);

  id = data.size()+1;

  data.push_back(0);

}

void Attribute::link(Attribute* dst) { 
  
  for (auto* link:dst->links) {
    
    if (link == this) return;
  
  }

  links.insert(dst); 
  
}

void Attribute::set(const float& val) {
 
  cur_val = val;

  update(val);

}

void Attribute::update(const float& val) {

  data[id] = val; //std::min(std::max(val,min_val),max_val);

  for (auto* link:links) link->update(get());

}

const float& Attribute::get() { return data[id]; }

Attribute::operator float() { return get(); }

void Attribute::operator= (const float& val) {  set(val); }

void Attribute::operator= (const int& val) {  set((float)val); }

