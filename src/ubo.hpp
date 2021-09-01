#ifndef UBO_H
#define UBO_H

#include "globals.hpp"

#include "shader.hpp"

struct UBO {

    static inline GLuint count = 0;

    std::vector<float> data;

    void* ptr;

    std::string name;

    GLuint id = 0;
    
    GLuint binding = count++;

    size_t ubo_size = 0;

    std::set<GL::ShaderProgram*> links;

    ~UBO() { destroy(); }

    UBO(std::string name = "ubo") : name(name) { } 

    UBO(void* data, size_t size, std::string name = "ubo") : ptr(data), name(name), ubo_size(std::ceil(size/4.0f)*4) {  create(); } 

    UBO(size_t size, std::string name = "ubo") : name(name), ubo_size(std::ceil(size/4.0f)*4) {

        data.resize(ubo_size);

        if (ubo_size) ptr = &data[0];

        create();

    } 

    void destroy() { if (id) glDeleteBuffers(1, &id); }

    void set(void* src) {  memcpy(&data[0], src, ubo_size);   }
    
    void set(void* src, size_t size = 0, int offset = 0) {   memcpy(&data[0]+offset, src, size);   }

    void create() {

        destroy();

         // can do better ^^
        if (binding > 100) std::cout << "MAX_UBO might soon be reached";

        if (name == "ubo") name += std::to_string(binding);

        glGenBuffers(1, &id);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, ubo_size, NULL, GL_DYNAMIC_COPY);

        GL_PRINT("layout(std140, binding = " << binding << ") uniform " << name << " { size:" << ubo_size << " };");

        for (auto l:links) { link(l); }
        
        
    }

    void link(GL::ShaderProgram* shader) {

        links.insert(shader);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader->id, glGetUniformBlockIndex(shader->id, name.c_str()), binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    }

    void resize(GLuint size) { 
        
        ubo_size = size;

        // std::vector<float>  t_data = std::move(data); 
        
        create(); 

        data.resize(size);
        // data = std::move(t_data); 

        if (size) ptr = &data[0];
        
    } 

    void send(){ send(ptr, ubo_size); }

    void send(GLvoid* data, size_t size, GLuint offset = 0){

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data); 

    }

};


#endif // UBO_H
