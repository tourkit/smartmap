#pragma once

#include "buffer.hpp"
#include "instance.hpp"
#include "model.hpp"

#include <cstdint>
#include <vector>

struct File;

struct VBO : Buffer {

    static inline Struct &vertice = Struct::create("Vertice").add<glm::vec2>("Position").add<glm::vec2>("UV").add<uint32_t>("ID");
    
    static inline Struct &indice = Struct::create("Indice").add<int>("vert1").add<int>( "vert2").add<int>("vert3");
    
    // static inline Struct &models = Struct::create("Models").add<File*>("file").add<int>("quantity");

    uint32_t vao, vbo, ibo;

    bool init = false;
    
    Struct vertices;
    Struct indices;

    VBO();

    ~VBO();

    void upload() override;
    void update() override;

    void create();
    void destroy();

    void draw(int count = 1);
       
    Model& add(File *file, int quantity = 1);

    std::vector<Model> models ;

};