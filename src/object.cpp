#include "object.hpp"

#include <memory>
#include <cstring>

#include "struct.hpp"
#include "buffer.hpp"
#include "entry.hpp"

size_t Object::size() { return s->size * reserved; }

char *Object::data(size_t id) { return &buffer->data[offset + (s->size * id)]; }

Entry &Object::push() { 

    std::vector<char> data;
    data.resize(s->size);
    memset(&data[0],0,data.size());

    return push(&data[0]);

}

Entry &Object::push(void* data, int id) { 

    reserved+=1;
    buffer->update();
    if (id>-1) memcpy(this->data()+s->size*id,data,s->size);
    else memcpy(this->data(),data,s->size*reserved);

    // if upscale rearange after rescale
    // if dowscale rearange before rescale

    if (!id) id = entrys.size();
    entrys.push_back(new Entry{this,id});
    return *entrys.back();

}