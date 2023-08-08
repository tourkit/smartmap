#ifndef UBO_H
#define UBO_H

#include "pch.hpp"

#include "shader.hpp"
#include "component.hpp"

struct UBO {

    static inline GLuint count = 0;

    GLuint binding = count++;

    std::vector<float> data;

    GLuint id = 0;

    std::vector<GLuint> subscribers;

    std::vector<Component*> definition;

    const char* name;

    ~UBO();

    UBO();
    
    UBO(const char* name, size_t size = 0, std::vector<GLuint> subscribers = {});

    void destroy();

    void resize(size_t size);

    void create(const char* name, size_t size, std::vector<GLuint> &subscribers);

    void link(GLuint shader);

    void update();

    void update(GLvoid* data, size_t size, GLuint offset = 0);

};


#endif // UBO_H
