#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>

struct Struct;
struct Buffer;


struct Object { 
    
    struct Entry {

        Object* obj;
        int id;

        const char* name();

        template <typename T>
        T& m(int id) { return *((T*)obj->data(id)); }

    };

    Struct* s; 

    int reserved; 

    Buffer* buffer;
    size_t offset;

    char *data(size_t id = 0);

    size_t size();

    Entry push();

    Entry push(void* data, int id = 0);

};

struct Buffer {
    
    std::vector<Object> objects;

    std::function<void()> callback;

    Buffer();

    Object* addObj(Struct* s, int reserved = 0);

    void update();

    void reset();
    
    void destroy();

    std::vector<char> data;  

    char* getEntry(Struct *s, int eq = 0);

    Object* operator[](int id) { return &objects[id]; }

};