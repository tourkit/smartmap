
#include "buffer2.hpp"
#include "instance2.hpp"

 TEST::Instance TEST::Buffer::operator[](const char* name) { return Instance{data.data(),0,this}[name]; }