#pragma once

#include "member.hpp"

#include <cstdint>

struct File;

struct VBO : Member {
    
    static inline auto vertice = Member("Vertice").add<float,2>("POSITION").range(-1,1,0).add<float,2>("UV").add<float,2>("NORMALIZED").add<float>("OBJ").add<float>("LAYER");

    static inline auto indice = Member("Indice").add<int>("vert1").add<int>( "vert2").add<int>("vert3");

    static inline struct Quad : File { Quad() : File("quad.obj", "o quad\n\nv -1 -1 0\nv 1 -1 0\nv -1 1 0\nv 1 1 0\n\nvt 0 0\nvt 1 0\nvt 0 1\nvt 1 1\n\nf 1/1 2/2 3/3 \nf 2/2 3/3 4/4") { } } quad;

    static inline std::vector<VBO*> pool;

    uint32_t vao = 0;

    uint32_t vbo = 0 , ibo = 0, enabled_attrs = 0;

    bool init = false;

    Member vertices;
    Member indices;

    VBO();

    VBO(File* file);

    ~VBO();

    void upload() override;
    void update();

    void create();
    void destroy();
    void reset();

    void draw(int count = 1);

    void addQuad(float w = 1, float h = 1, float x = 0, float y = 0, int id = 0);

    bool addFile(File *file = &quad, int id = 0);
    bool addFile_noupload(File *file = &quad, int id = 0);

    int layer_id = 0;

};
