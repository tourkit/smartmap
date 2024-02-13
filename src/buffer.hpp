#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

#include "object.hpp"

struct Struct;

struct BufferOwner {

    virtual void update() { }
    virtual void upload() { }

};


struct Buffer {
    
    std::vector<Object> objects;

    std::function<void()> callback;

    BufferOwner *owner = nullptr;

    Buffer();
    
    Buffer(BufferOwner* owner) : owner(owner) {}

    Object* addObj(Struct* s, int reserved = 0);

    void update();

    void reset();
    
    void destroy();

    std::vector<char> data;  

    Object* operator[](int id) { return &objects[id]; }

};