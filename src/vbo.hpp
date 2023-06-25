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

    ~VBO(); 

    void import(std::string path);

    void update();
    void destroy();
    void reset();

    VBO(std::string path = "assets/model/quad.obj") ;

    void draw(int count = 1);

};

#endif