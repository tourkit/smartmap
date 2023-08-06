# pragma once

#include "pch.hpp"

struct Attribute {

  std::string name;

  std::vector<Attribute> childrens;

  float range_from, range_to, default_value, range_step;

  float *data;

  Attribute(std::string name, float range_from = 0, float range_to = 1, float default_value = 1) ;
  
  Attribute(std::string name, float *ptr, float range_from = 0, float range_to = 1, float default_value = 1) ;

  Attribute(std::string name, std::vector<Attribute> childrens);

  Attribute* child(std::string name);
 
  Attribute* operator[] (uint16_t id);

};
