#include "log.hpp" 
#include "buffer.hpp" 
#include "struct.hpp" 
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

    auto backup_objects = objects; // should remode last ?

    auto backup_data = data;

    int offset = 0;
    for (auto obj : objects) {

        obj.offset = offset;
        offset += obj.size();
        
    }

    data.resize(offset);
    memset(&data[0],0,data.size());

    // transpose old data

}

char* Buffer::getEntry(Struct *s, int eq) {
    
    for (auto &obj : objects) {

        if (obj.s == s) {

            return obj.data() + obj.size() ;

        }

    }

    return nullptr;

} 