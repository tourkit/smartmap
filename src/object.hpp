#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "struct.hpp"
#include "entry.hpp"

struct Buffer;

struct Object { 

    Struct* s; 

    int reserved; 

    Buffer* buffer;
    size_t offset;
    
    bool is_striding = true;

    char *data(size_t id = 0);
    int  eq(size_t id = 0);

    size_t size();

    size_t stride();

    std::vector<Entry*> entrys;
    
    Entry &push();

    Entry &push(void* data);

    void addComp(std::string components);
    void removeComp(std::string components);

    void clear();

};