#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

struct Struct;

struct Buffer {

    struct Object { 
        
        Struct* s; 

        int reserved = 1; 

        size_t data_offset;

        // std::vector<Component*> eq; 
        // struct Entry { int id, offset; };	

    };

    std::vector<Object> objects;

    std::function<void()> callback;

    void addObj(Struct* s, int reserved = 1);

    void reset();
    
    void destroy();

    std::vector<char> data;  

};