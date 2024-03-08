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

        Instance operator[](const char* name);

        bool exist();

        // void set
        
    };



};

