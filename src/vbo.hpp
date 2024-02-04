#pragma once

#include "struct.hpp"
#include <cstdint>
#include <vector>


struct Object;
struct File;

struct VBO {

    static inline Object *vertices;
    static inline Object *indices;

    uint32_t vao, vbo, ibo;
    
    VBO();

    ~VBO();

    void upload();

    void create();
    void destroy();

    void draw(int count = 1);
       
    void import(File *file);

};