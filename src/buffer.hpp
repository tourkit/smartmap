#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

#include "object.hpp"

struct Struct;

struct Buffer {
    
    std::vector<Object> objects;

    std::vector<char> data;  

    std::function<void()> callback;
    
    Buffer();

    Object* addObj(Struct* s, int reserved = 0);

    virtual void update() { }

    virtual void upload() { }

    void reset();
    
    void destroy();

    Object* operator[](int id) { return &objects[id]; }

};