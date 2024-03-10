#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "buffer2.hpp"

namespace TEST {


    struct Buffer;
    struct AnyMember;

    struct Instance { 

        char* data;
        int offset;
        AnyMember* member = nullptr;
        int id = 0;

        Instance operator[](const char* name);
        Instance operator[](int id);

        bool exist();


        Instance& eq(int id);

        template <typename T>
        T get() { return *((T*)(data+offset)); }

        template <typename T>
        Instance& set(T val) {

            memcpy(data+offset, &val, member->size());

            return *this;

        }
        
    };

};

