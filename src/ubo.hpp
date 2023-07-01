#ifndef UBO_H
#define UBO_H

#include "globals.hpp"

#include "shader.hpp"

struct UBO {

    static inline GLuint count = 0;

    GLuint binding = count++;

    std::vector<float> data;

    GLuint id = 0;

    const char* name;

    ~UBO() { destroy(); }

    UBO(const char* name, size_t size = 0, std::vector<GLuint> subscribers = {}) : name(name) { 

        data.resize(size);

        create(); 

        for (auto shader:subscribers) link(shader); 
        
    } 

    void destroy() { if (id) glDeleteBuffers(1, &id); }

    void create() {

        destroy();

         // can do better ^^
        if (binding > 100) std::cout << "MAX_UBO might soon be reached";

        glGenBuffers(1, &id);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*data.size(), NULL, GL_DYNAMIC_COPY);

        std::cout << "RTFM /!\\ put good bindings in shader !! layout(std140, binding = " << binding << ") uniform " << name << " { size:" << data.size() << " };" << std::endl;
        
    }

    void link(GLuint shader) {

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, name), binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    }


    void update(){ update(&data[0], data.size()); }

    void update(GLvoid* data, size_t size, GLuint offset = 0){

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data); 

    }

};


#endif // UBO_H
