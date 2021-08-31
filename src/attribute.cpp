#include "attribute.hpp"  


#include <unordered_set>
#include <string>

Number::~Number() { 
  
  for (auto* attr:pool) { attr->links.erase(this); }  
  
  pool.erase(this); 
  
}


<<<<<<< Updated upstream

Attribute::Attribute(float v) : Attribute() { 
  
  set(v); 
=======
Number::Number() { }
>>>>>>> Stashed changes

Number::Number(float v) : Number() { set(v); }
Number::Number(int v) : Number() { set((float)v); }

<<<<<<< Updated upstream
Attribute::Attribute(std::vector<float>* data = nullptr) {
=======
ShaderNumber::ShaderNumber(float v) :  Number(v) { ShaderNumber(); }
ShaderNumber::ShaderNumber(int v) : Number(v) { ShaderNumber(); }
>>>>>>> Stashed changes

ShaderNumber::ShaderNumber() {

  id = data.size()+1;

  data.push_back(0);

}

void Number::link(Number* dst) { 
  
  for (auto* link:dst->links) {
    
    if (link == this) return;
  
  }

  links.insert(dst); 
  
}

void Number::set(const float& val) {
 
  cur_val = val;

  update(val);

}

void Number::update(const float& val) {

  for (auto* link:links) ((Number*)link)->update(get());

}

void ShaderNumber::update(const float& val) {

  data[id] = val; //std::min(std::max(val,min_val),max_val);

  Number::update(val);

}

const float& Number::get() { return cur_val; }

Number::operator float() { return get(); }

void Number::operator= (const float& val) {  set(val); }

void Number::operator= (const int& val) {  set((float)val); }
