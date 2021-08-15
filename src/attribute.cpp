#include "attribute.hpp"  

Attribute::Attribute(float v) : Attribute() { set(v); }

Attribute::Attribute(int v) : Attribute() { set((float)v); }

Attribute::Attribute() {

  cur_val = 0;

  link_from = nullptr;

  id = attributes.size();

  attributes.push_back(0);

}

Attribute::~Attribute() {

  if (link_from) link_from->unlink(this);

  for (auto* l:links_to) unlink(l);

}

void Attribute::link(Attribute* dst) {

  links_to.insert(dst);

  dst->link_from = this;

  dst->update(attributes[id]);

}

void Attribute::unlink(Attribute* dst) {

  links_to.erase(dst);

  dst->link_from = nullptr;

  dst->update(dst->cur_val);

}

void Attribute::set(const float& val) {

  cur_val = val;

  update(val);

}

void Attribute::update(const float& val) {

  attributes[id] = val;//std::min(std::max(val,min_val),max_val);

  for (auto* l:links_to) attributes[l->id] = attributes[id];

  // + callback() ?

}

const float& Attribute::get() { return attributes[id]; }

Attribute::operator float() { return get(); }

void Attribute::operator= (const float& val) {  set(val); }

void Attribute::operator= (const int& val) {  set((float)val); }
