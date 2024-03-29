#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "object.hpp"

struct Struct;
struct Buffer {
    
    static inline std::map<Buffer*,Buffer> bkps;

    std::vector<Object> objects;

    std::vector<char> data;  

    std::function<void()> callback;
    
    Buffer();
    
    Buffer* bkp();

    virtual Object* addObj(Struct* s, int reserved = 0);

    virtual void update() { }

    virtual void upload() { }

    void reset();
    
    void destroy();

    void remap(Buffer* bkp);

    Object* operator[](int id) { return &objects[id]; }


};