#include "remap.hpp"
#include "member.hpp"

Remap::Remap(Instance *src , Instance *dst, int quantity) : src(src) ,dst(dst) , quantity(quantity) {
    
    src->track(); ADD_UNIQUE<Remap*>(src->remaps, this ); 
    dst->track(); ADD_UNIQUE<Remap*>(dst->remaps, this );  

}

void Remap::update() {

    int size = src->stl.back().m->size();

    if (dst->stl.back().m->size()<size) size = dst->stl.back().m->size();

    memcpy(dst->data(), src->data(), quantity * size);


}