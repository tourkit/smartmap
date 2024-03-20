#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "buffer.hpp"

struct Buffer;
struct Member;

struct Instance { 

    Buffer* buff;
    uint32_t offset;
    Member* member = nullptr;
    uint32_t id = 0;

    Instance operator[](std::string name);
    Instance operator[](int id);

    bool exist();

    char* data() { return buff->data.data()+offset; }
    uint32_t size() { return member->footprint_all(); }

    Instance eq(int id);

    template <typename T>
    T get() { return *((T*)(data())); }

    template <typename T>
    Instance& set(T val) {
        
        memcpy(data(), &val, sizeof(T));

        return *this;

    }

    Instance& set(void* ptr, size_t size) {

        memcpy(data(), ptr, size);

        return *this;

    }

    Instance push(void* ptr = nullptr, size_t size = 0) {

        member->quantity(member->quantity()+1);

        auto inst = eq(member->quantity()-1);

        if (ptr) {
        
            if (!size) size = member->size();
            
            inst.set(ptr,size);

        }

        return inst;

    }
    
};