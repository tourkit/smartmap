#pragma once

#include "struct.hpp"
#include "buffer.hpp"
#include <cstdint>
#include <vector>

struct Model;
struct File;

struct VBO : Buffer {

    static inline Struct &vertices_s = Struct::create("Vertex").add("Position").add("UV").add("ID");//.striding(true);
    static inline Struct &indices_s = Struct::create("Index").add("Vertex").add( "Vertex").add("Vertex");//.striding(true);

    uint32_t vao, vbo, ibo;
    
    Struct *vertices;
    Struct *indices;
    
    VBO();

    ~VBO();

    void upload() override;
    void update() override;

    void create();
    void destroy();

    void draw(int count = 1);
       
    int import(File *file);

    std::vector<Model*> models;

};