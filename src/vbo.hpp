#pragma once

#include "struct.hpp"
#include <cstdint>
#include <vector>


struct Object;
struct Model;

struct VBO {

    static inline Object *vertices;
    static inline Object *indices;

    uint32_t vao, vbo, ibo;

    std::vector<std::pair<Model*,int>> models;
    
    VBO();

    ~VBO(); 

    void add(Model *model, int id = 0);

    void upload();

    void create();
    void destroy();

    void draw(int count = 1);

};