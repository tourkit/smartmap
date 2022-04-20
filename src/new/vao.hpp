#ifndef SMARTMAP_VAO_HPP
#define SMARTMAP_VAO_HPP
#pragma once

#include "src/window.hpp"

struct VAO {

    unsigned int id, vertices, indices;

    ~VAO() { glDeleteBuffers(1, &vertices); glDeleteBuffers(1, &indices); glDeleteVertexArrays(1, &id);  }

    VAO() { glGenBuffers(1, &vertices); glGenBuffers(1, &indices); glGenVertexArrays(1, &id); }

    void send(){//flecs::entity e) {
 
        glBindVertexArray(id); 

        // glBindBuffer(GL_ARRAY_BUFFER, vertices);
        // glBufferData(GL_ARRAY_BUFFER,  mesh.vertices.size()*sizeof(Vertice) , &mesh.vertices[0], GL_DYNAMIC_DRAW );

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size()*sizeof(Indice) , &mesh.indices[0], GL_DYNAMIC_DRAW );

       // decrypt some component (said to be a vertice) into corresponding glVertexAttribPointer

        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (GLvoid *) 0);
        // glEnableVertexAttribArray(0); 

    }
    
};


#endif