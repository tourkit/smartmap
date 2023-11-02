#ifndef VBO_H
#define VBO_H

#include "pch.hpp"
#include "gui.hpp"
#include "buffer.hpp"
	
struct VBO {

    static inline std::vector<VBO*> pool;

    std::string name;

    Buffer buffer;
    Buffer::Object *vertices, *indices;

    GLuint vao=0, vbo, ibo, id;

    ~VBO(); 

    void import(std::string path);

    void upload();
    void destroy();

    VBO(std::string path = "quad.obj", int id = 0, std::string name = "VBO") ;

    void draw(int count = 1);



};

#endif