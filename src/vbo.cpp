#include "vbo.hpp"  

VBO::VBO() {    

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

}

void VBO::upload() {

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices.size()*sizeof(Vertice) , &vertices[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(Indice) , &indices[0], GL_STATIC_DRAW );

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_TRUE, sizeof(Vertice), (GLvoid *) (2*sizeof(float)));
    glEnableVertexAttribArray(1);

}

void VBO::addQuad(int id) {

    int pos = indices.size()*sizeof(Indice);

    vertices.push_back({0, 1, id});
    vertices.push_back({1, 1, id});
    vertices.push_back({0, 0, id});
    vertices.push_back({1, 0, id});

    indices.push_back({pos+0,pos+1,pos+2});
    indices.push_back({pos+1,pos+2,pos+3});

    upload();

}

void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, indices.size()*sizeof(Indice), GL_UNSIGNED_INT, 0, count);

}
	
