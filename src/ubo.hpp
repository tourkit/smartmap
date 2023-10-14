#ifndef UBO_H
#define UBO_H

#include "pch.hpp"

#include "shader.hpp"

#include "buffer.hpp"

struct UBO {

    std::string name;

    static inline std::vector<UBO*> pool;

    GLuint id;

    GLuint binding;

    static inline GLuint binding_count = 0;

    static inline Components &comps = Components::getInstance();

    std::vector<ShaderProgram*> subscribers;

    Buffer buffer;

    ~UBO();
    
    UBO(std::string name, std::vector<ShaderProgram*> subscribers = {});

    void destroy();

    void update();

    void upload();

    void upload(GLvoid* data, size_t size, GLuint offset = 0);
    
    // static void toJSON();
    
    // static void fromJSON();

};
#endif // UBO_H
