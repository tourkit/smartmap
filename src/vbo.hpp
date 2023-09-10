#ifndef VBO_H
#define VBO_H

#include "pch.hpp"
#include "gui.hpp"
#include "buffer.hpp"
	
struct VBO {

    static inline std::vector<VBO*> pool;

    Buffer buffer;
    Buffer::Object *vertices, *indices;

    GLuint vao=0, vbo, ibo, id;

    uint16_t width, height;

    ~VBO(); 

    void import(std::string path, uint16_t width, uint16_t height);

    void update();
    void destroy();

    VBO(std::string path = "quad.obj", uint16_t width=0, uint16_t height=0) ;

    void draw(int count = 1);

    
    struct Vertice { float pos_x,pos_y; float tex_x,tex_y; float clip_x,clip_y; GLuint id; }; 

    std::vector<Vertice> vx;

    struct Indice { int a,b,c; }; 

    std::vector<Indice> ix;



};

#endif