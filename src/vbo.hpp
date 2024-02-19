#pragma once

#include "struct.hpp"
#include "buffer.hpp"
#include "model.hpp"
#include <cstdint>
#include <vector>

struct Object;
struct Model;

struct VBO : Buffer {

    static inline Struct *vertices_s = new Struct("Vertex", {"Position","UV","ID",});
    static inline Struct *indices_s = new Struct("Index",{"Vertex", "Vertex", "Vertex"});

    uint32_t vao, vbo, ibo;
    
    Object *vertices;
    Object *indices;
    
    VBO();

    ~VBO();

    void upload() override;
    void update() override;

    void create();
    void destroy();

    void draw(int count = 1);
       
    int import(Model *model);

    std::vector<Model*> models;

};