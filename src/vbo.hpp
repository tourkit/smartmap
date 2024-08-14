#pragma once

#include "buffer.hpp"
#include "instance.hpp"

#include <cstdint>
#include <list>

struct File;

struct VBO : Buffer {

    static inline struct Quad : File { Quad() : File("quad.obj", "o quad\n\nv -1 -1 0\nv 1 -1 0\nv -1 1 0\nv 1 1 0\n\nvt 0 0\nvt 1 0\nvt 0 1\nvt 1 1\n\nf 1/1 2/2 3/3 \nf 2/2 3/3 4/4") { } } quad;

    static inline Struct *vertice = &Struct::create("Vertice").add<glm::vec2>("POSITION").range(-1,1,0).add<glm::vec2>("UV").add<glm::vec2>("NORMALIZED").add<float>("OBJ").add<float>("LAYER");

    static inline Struct *indice = &Struct::create("Indice").add<int>("vert1").add<int>( "vert2").add<int>("vert3");

    static inline std::vector<VBO*> pool;

    uint32_t vao = 0;

    uint32_t vbo = 0 , ibo = 0, enabled_attrs = 0;

    bool init = false;

    Struct vertices;
    Struct indices;

    VBO();

    VBO(File* file);

    ~VBO();

    void upload() override;
    void update() override;

    void create();
    void destroy();
    void reset();

    void draw(int count = 1);

    void addQuad(float w = 1, float h = 1, float x = 0, float y = 0, int id = 0);

    bool add(File *file = &quad, int id = 0);
    bool add_noupload(File *file = &quad, int id = 0);

    int layer_id = 0;

};
