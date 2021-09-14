#ifndef VBO_H
#define VBO_H

#include "globals.hpp"
	
struct VBO {

    struct Vertice { float pos_x,pos_y; float tex_x,tex_y; float clip_x,clip_y; int id; }; 
    struct Indice { int a,b,c; }; 

    std::vector<Vertice> vertices;

    std::vector<Indice> indices;

    GLuint vao, vbo,ibo;

    ~VBO(); // that should be elsewhere I think

    VBO();    
    
    void upload();

    void addQuad(int id = 0);

    void draw(int count = 1);

} ;

struct Quad : public VBO { Quad(int id = 0) { addQuad(id);} };


#endif