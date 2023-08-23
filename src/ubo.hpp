#ifndef UBO_H
#define UBO_H

#include "pch.hpp"

#include "shader.hpp"

#include "buffer.hpp"

struct UBO {

    std::string name;

    GLuint id;

    GLuint binding;

    static inline GLuint binding_count = 0;

    static inline Components &comps = Components::getInstance();

    std::vector<Shader*> subscribers;

    Buffer buffer;

    ~UBO();
    
    UBO(std::string name, std::vector<Shader*> subscribers = {});

    void destroy();
    void resize();

    void link(GLuint shader);

    void update();
    
    // static void toJSON();
    
    // static void fromJSON();

    void update(GLvoid* data, size_t size, GLuint offset = 0);

};
#endif // UBO_H
