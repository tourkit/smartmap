#ifndef VBO_H
#define VBO_H

#include "pch.hpp"
#include "gui.hpp"
#include "component.hpp"
	
struct VBO {

    static inline std::vector<VBO*> pool;

    std::string path;

    struct Vertice { float pos_x,pos_y; float tex_x,tex_y; float dim_x,dim_y; GLuint id; }; 

    
    Object vertex{"Vertex", {

        Component::id("Position"),
        Component::id("UV"),
        Component::id("Dimentions"),
        Component::id("id")
        
    }, 0};

    Object index{"Index", {

        Component::id("Vertex"),
        Component::id("Vertex"),
        Component::id("Vertex"),
        
    }, 0};

    struct Indice { int a,b,c; }; 

    GLuint vao=0, vbo, ibo, id;

    uint16_t width, height;

    std::vector<Vertice> vertices;

    std::vector<Indice> indices;

    ~VBO(); 

    void import(std::string path, uint16_t width, uint16_t height);

    void update();
    void destroy();
    void reset();

    VBO(std::string path = "quad.obj", uint16_t width=0, uint16_t height=0) ;

    void draw(int count = 1);


};

#endif