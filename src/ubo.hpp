#ifndef UBO_H
#define UBO_H

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

#include "shader.hpp"

static std::vector<float> attributes;

struct UBO {

    static inline GLuint count = 0;

    GLuint id, binding_point;

    UBO() { id = 0; }

    ~UBO() { destroy(); }

    UBO(GLuint size) { create(size); }

    void create(int size = 0) {

        destroy();

        if (!size) {

            attributes.resize(std::ceil(attributes.size()/4.0f)*4);

            size = attributes.size();

        }

        size = std::ceil(size/4.0f)*4;

        glGenBuffers(1, &id);

        binding_point = count;

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, size*sizeof(float), NULL, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, id);

        if (binding_point > 100) std::cout << "MAX_UBO might soon be reached, replace with proper glGet here";

    }

    void destroy() { if (id) glDeleteBuffers(1, &id); }

    void resize(GLuint size) { destroy(); create(size); } // need to create new UBO cause i Believe glBufferData can't be resized

    void send(){ send(&attributes[0], sizeof(float)*attributes.size()); }

    void send(GLvoid* data, size_t size, GLuint offset = 0){

        // to replace with subData 
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr+offset, data, size);
        glUnmapBuffer(GL_UNIFORM_BUFFER);

    }

    void link(const GL::ShaderProgram& shader, std::string name = "data") {

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader.id, glGetUniformBlockIndex(shader.id, name.c_str()), binding_point);

    }

};


#endif // UBO_H
