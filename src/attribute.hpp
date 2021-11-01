#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <vector>
#include <set>
#include <iostream>
#include <string.h>


#include "gui.hpp"


struct Attribute {

  static inline std::vector<float> UBO;

  int id = -1;

  const char* name;

  std::vector<Attribute*> childrens;

  float min_val, max_val, def_val, range_val;
 
  std::vector<float> local{1}; 

  std::vector<float>* buffer = &UBO; 

  Attribute(const char* name, float val, float min_val = 0, float max_val = 1, std::vector<float>* dst = nullptr) 
  
    : name(name), min_val(min_val), max_val(max_val), def_val(val), range_val(max_val-min_val) { 

      if (dst) buffer = dst; 

      id = buffer->size();
      
      buffer->push_back(0);

      set(val);
    
  }

  void gui() {

    int size = childrens.size();
    if (!size) size = 1;

    if (size == 1) ImGui::SliderFloat(name,ptr(),min_val,max_val); 
    else if (size == 2) ImGui::SliderFloat2(name,ptr(),child(0)->min_val,child(0)->max_val); 
    else if (size == 3) ImGui::SliderFloat3(name,ptr(),child(0)->min_val,child(0)->max_val); 
    else if (size == 4) ImGui::SliderFloat4(name,ptr(),child(0)->min_val,child(0)->max_val); 

  }

  float* ptr() { if (id <0) return child(0)->ptr(); else return &(*buffer)[id]; }

  Attribute(const char* name, std::set<Attribute*> childrens, float min_val = 0, float max_val = 1) : name(name) { for (auto c:childrens) this->childrens.push_back(c); }

  Attribute(std::set<Attribute*> childrens) : Attribute("", childrens) { }

  Attribute* link_src = nullptr;

  std::set<Attribute*> links_dst;

  float get() { return (*buffer)[id]; }

  virtual void set(float val) { (*buffer)[id] = val;  for (auto l:links_dst) l->set(val); } 

  void setNormalized(float val) { set(val*(range_val)+min_val); } 

  Attribute* child(int id = 0) { return childrens[id]; }

  Attribute* child(const char* name) { 
    
      Attribute* output = this;

      char* t_attr = strdup(name);
    
      char * pch = strtok (t_attr, " ,.-");

      while (pch != NULL) {

        Attribute* found = nullptr;
      
        for (auto c:output->childrens) { 
          
          if (!strcmp(pch,c->name)) {
            
              found = c; 
              break;

          } 

        } 

        if (!found) std::cout << "\"" << pch << "\"" << " not found !" << std::endl; 
        else output = found;

        pch = strtok (NULL, ",.-");
      
      }

      free(t_attr);

    if (output == this) return nullptr;
    else return output; 
    
    }

  void linkFrom(Attribute* src) {

    src->links_dst.insert(this);

    link_src = src;

  }

  void linkTo(Attribute* dst) { dst->linkFrom(this); }

  ~Attribute() { for (auto l:links_dst) l->link_src = nullptr; }

};




#endif