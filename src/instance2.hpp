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

        Buffer* buffer;
        int offset;
        AnyMember* member = nullptr;
        int id = 0;

        Instance operator[](const char* name);

        bool exist();


        Instance& eq(int id);

        template <typename T>
        T get() { return *((T*)&buffer->data[offset]); }

        template <typename T>
        Instance& set(T val) {

            memcpy(&buffer->data[offset], &val, member->size());

            return *this;

        }
        
    };

};

