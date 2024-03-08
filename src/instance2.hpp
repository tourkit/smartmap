#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>


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

        // template <typename T>
        // Instance& set(T val) {}
        
    };



};

