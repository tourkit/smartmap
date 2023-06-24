#ifndef VBO_H
#define VBO_H

#include "globals.hpp"
	
struct VBO {

    static inline std::vector<VBO*> pool;

    struct Vertice { float pos_x,pos_y; float tex_x,tex_y; float clip_x,clip_y; GLuint id; }; 

    struct Indice { int a,b,c; }; 

    std::vector<Vertice> vertices;

    std::vector<Indice> indices;

    GLuint vao, vbo,ibo, id;

    ~VBO(); 

    void upload();

    VBO(std::string path = "assets/model/quad.obj") ;

    void draw(int count = 1);

};

#endif