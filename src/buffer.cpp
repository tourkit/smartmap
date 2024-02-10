#include "log.hpp" 
#include "buffer.hpp" 
#include "struct.hpp" 
#include "entry.hpp" 
#include <cstring>


Buffer::Buffer() { objects.reserve(1000); }

void Buffer::reset() { destroy(); }

void Buffer::destroy() { objects.resize(0); data.resize(0); }

Object* Buffer::addObj(Struct* s, int reserved) {

    if (objects.size()==999) PLOGW << "obj reserve reached";

    objects.push_back({s,reserved,this,data.size()});

    update();

    return &objects.back();

}

void Buffer::update() {

    std::vector<char> future_data = data;

}