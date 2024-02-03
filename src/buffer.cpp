#include "buffer.hpp" 
#include "struct.hpp" 
#include <cstring>

void Buffer::reset() { destroy(); }

void Buffer::destroy() { objects.resize(0); data.resize(0); }

void Buffer::addObj(Struct* s, int reserved) {

    auto backup_objects = objects;
    auto backup_data = data;

    objects.push_back({s,reserved,data.size()});

    auto &obj = objects.back();
    data.resize(data.size()+obj.s->size * obj.reserved);

    memset(&data[0],0,data.size());
    // transpose old data

}