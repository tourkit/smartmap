#pragma once

#include "struct.hpp"
#include "buffer.hpp"
#include <cstdint>
#include <vector>


struct Object;
struct File;

struct VBO {

    static inline Struct *vertices_s = new Struct("Vertex", {"Position","UV","ID",});
    static inline Struct *indices_s = new Struct("Index",{"Vertex", "Vertex", "Vertex"});

    static inline Buffer buffer;
    uint32_t vao, vbo, ibo;
    
    Object *vertices;
    Object *indices;
    
    VBO();

    ~VBO();

    void upload();

    void create();
    void destroy();

    void draw(int count = 1);
       
    void import(File *file);

};