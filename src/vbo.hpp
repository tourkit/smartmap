#pragma once

#include "buffer.hpp"
#include "instance.hpp"

#include <cstdint>
#include <vector>

struct Model;
struct File;

struct VBO : Buffer {

    static inline Struct &vertice = Struct::create("Vertice").add<glm::vec2>("Position").add<glm::vec2>("UV").add<uint32_t>("ID");
    
    static inline Struct &indice = Struct::create("Indice").add<uint32_t>("Vertice1").add<uint32_t>( "Vertice2").add<uint32_t>("Vertice3");

    uint32_t vao, vbo, ibo;
    
    Instance vertices;
    Instance indices;

    bool init = false;
    
    Struct vertice_array;
    Struct indice_array;

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