#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

struct Struct;
struct Buffer;

struct Object { 
    
    Struct* s; 

    int reserved; 

    Buffer* buffer;
    size_t offset;

    struct Entry{};

    char *data();

    size_t size() { return eq(reserved); }

    size_t eq(size_t id);

    void push(void* data) { 

        reserved+=1;


    }

};

struct Buffer {

    
    std::vector<Object> objects;

    std::function<void()> callback;

    Object* addObj(Struct* s, int reserved = 0);

    void update();

    void reset();
    
    void destroy();

    std::vector<char> data;  

    char* getEntry(Struct *s, int eq = 0);

};