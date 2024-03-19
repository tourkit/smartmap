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
    int offset;
    Member* member = nullptr;
    int id = 0;

    Instance operator[](const char* name);
    Instance operator[](int id);

    bool exist();


    Instance& eq(int id);

    template <typename T>
    T get() { return *((T*)(buff->data.data()+offset)); }

    template <typename T>
    Instance& set(T val) {

        memcpy(buff->data.data()+offset, &val, sizeof(T));

        return *this;

    }

    Instance& push(void* data, size_t size) {
        
        member->resize(member->quantity()+1);
        // buff->update();
        return *this;
    }
    
};