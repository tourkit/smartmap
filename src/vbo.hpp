#pragma once

#include "struct.hpp"

struct VBO {

    static inline Struct *vertices;
    static inline Struct *indices;

    uint32_t vao, vbo, ibo, id;

    std::string path;
    
    VBO(std::string path = "quad.obj", int id = -1);

    ~VBO(); 

    void import(std::string path);

    void upload();

    void destroy();

    void draw(int count = 1);

};