#ifndef UBO_H
#define UBO_H

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

#include "shader.hpp"

struct UBO {

    static inline GLuint count = 0;

    std::vector<float> data;

    void* ptr;

    std::string name;

    GLuint id, binding_point, size;

    ~UBO() { destroy(); }

    UBO(void* data, size_t size, std::string name = "data") : name(name) { 
        
        create(size); 

        ptr = data;

    } 

    UBO(GLuint size = 512, std::string name = "data") : name(name) { 

         // needs to be power of 4 
         // for STD140 data structure
        data.resize(std::ceil(size/4.0f)*4);

        UBO(&data[0],data.size(),name);

      } 

    void destroy() { if (id) glDeleteBuffers(1, &id); }

    void set(void* src) {  memcpy(&data[0], src, this->size);   }
    
    void set(void* src, size_t size = 0, int offset = 0) {   memcpy(&data[0]+offset, src, size);   }

    void create(size_t size) {

        this->size = size;

        destroy();

        glGenBuffers(1, &id);

        binding_point = count;

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, size*sizeof(float), NULL, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, id);

        if (binding_point > 100) std::cout << "MAX_UBO might soon be reached, replace with proper glGet here";

    }

    void resize(GLuint size) { 
        
        std::vector<float>  t_data = std::move(data); 
        
        destroy(); create(size); // need to create new UBO_bufferid cause i Believe glBufferData can't be resized
        
        data = std::move(t_data); 
        
    } 

    void send(){ send(ptr, sizeof(float)*size); }

    void send(GLvoid* data, size_t size, GLuint offset = 0){

        // to replace with subData 
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(static_cast<char*>(ptr)+offset, data, size);
        glUnmapBuffer(GL_UNIFORM_BUFFER);

    }

    void link(const GL::ShaderProgram& shader, std::string name = "data") {

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader.id, glGetUniformBlockIndex(shader.id, name.c_str()), binding_point);

    }

};


#endif // UBO_H
