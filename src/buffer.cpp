
#include "buffer.hpp"
#include "instance.hpp"
// #include "engine.hpp"


Instance Buffer::operator[](std::string name) { return Instance{this,0,this}[name]; }
Instance Buffer::operator[](int id) { return Instance{this,0,this}[id]; }
