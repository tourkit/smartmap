#ifndef VBO_H
#define VBO_H

#include "globals.hpp"
	
struct VBO {

    static inline std::vector<VBO*> pool;

    std::string path;

    struct Vertice { float pos_x,pos_y; float tex_x,tex_y; float clip_x,clip_y; GLuint id; }; 

    struct Indice { int a,b,c; }; 

    std::vector<Vertice> vertices;

    std::vector<Indice> indices;

    GLuint vao=0, vbo,ibo, id;

    uint16_t width, height;

    ~VBO(); 

    void import(std::string path);

    void update();
    void destroy();
    void reset();

    VBO(std::string path = "quad.obj", int id = 0, uint16_t width=1, uint16_t height=1) ;

    void draw(int count = 1);

};

#endif