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



//// BUFFER::OBJECT

const char* Object::Entry::name() { return obj->s->name.c_str(); }

size_t Object::size() { return s->size * reserved; }

char *Object::data(size_t id) { return &buffer->data[offset + (s->size * id)]; }

Object::Entry Object::push() { 

    std::vector<char> data;
    data.resize(s->size);
    memset(&data[0],0,data.size());

    return push(&data[0]);

}

Object::Entry Object::push(void* data, int id) { 

    reserved+=1;
    buffer->update();
    if (id>-1) memcpy(this->data()+s->size*id,data,s->size);
    else memcpy(this->data(),data,s->size*reserved);

    // if upscale rearange after rescale
    // if dowscale rearange before rescale

    return {this,id};

}