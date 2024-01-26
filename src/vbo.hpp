#pragma once

#include "node.hpp"
#include "buffer.hpp"

struct Models;
struct Struct;

struct VBO {

    static inline std::vector<VBO*> pool;

    std::string name;

    Buffer buffer;
    Struct *vertices, *indices;

    uint32_t vao=0, vbo, ibo, id;

    std::vector<Models*> models;
    
    ~VBO(); 

    void import(std::string path);

    void upload();
    void destroy();

    VBO(std::string path = "quad.obj", int id = 0, std::string name = "VBO") ;

    void draw(int count = 1);

};