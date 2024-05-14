#pragma once

#include "buffer.hpp"
#include "instance.hpp"
#include "model.hpp"

#include <cstdint>
#include <list>

struct File;

struct VBO : Buffer {

    static inline struct Quad : File { Quad() { data = "o quad\n\nv -1 -1 0\nv 1 -1 0\nv -1 1 0\nv 1 1 0\n\nvt 0 1\nvt 1 1\nvt 0 0\nvt 1 0 \n\nf 1/1 2/2 3/3 \nf 2/2 3/3 4/4"; } } quad;

    static inline Struct *vertice = &Struct::create("Vertice").add<glm::vec2>("Position").add<glm::vec2>("UV").add<int>("ID");

    static inline Struct *indice = &Struct::create("Indice").add<int>("vert1").add<int>( "vert2").add<int>("vert3");

    static inline uint32_t vao = 0;

    uint32_t vbo = 0 , ibo = 0, enabled_attrs = 0;

    bool init = false;

    Struct vertices;
    Struct indices;

    VBO();

    ~VBO();

    void upload() override;
    void update() override;

    void create();
    void destroy();
    void reset();

    void draw(int count = 1);

    bool add(File *file = &quad, int id = 0); // chelou push la, devrait etre ds inst ou inverse,

};
