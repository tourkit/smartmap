#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "struct2.hpp"



namespace TEST {

    struct Instance;

    struct Buffer : Struct{

        std::vector<char> data;
        
        Buffer() : Struct("buffer_definition") {

            data.reserve(1000);
            memset(data.data(),0,data.size());

        }

        void set(AnyMember& m, void* data) { }

        void update() override { data.resize( footprint_all() ); Struct::update(); }

        

        Instance operator[](const char* name);

    };

    




};