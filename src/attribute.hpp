#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <vector>
#include <set>
#include <iostream>
#include <string.h>

struct Attribute {

  static inline std::vector<float> UBO;

  int id = -1;

  const char* name;

  std::vector<Attribute*> childrens;

  Attribute(float val, const char* name = "") : name(name) { 

    id = UBO.size();
    
    UBO.push_back(0);

    set(val);
    
  }

  Attribute(std::set<Attribute*> childrens, const char* name = "") : name(name) { 

    for (auto c:childrens) this->childrens.push_back(c);
    
  }

  Attribute* link_src = nullptr;

  std::set<Attribute*> links_dst;

  float get() { return UBO[id]; }

  void set(float val) { UBO[id] = val;  for (auto l:links_dst) l->set(val); } 

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