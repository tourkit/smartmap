#ifndef UBO_H
#define UBO_H

#include "pch.hpp"

#include "shader.hpp"
#include "component.hpp"

struct UBO {

    static inline std::vector<UBO*> pool;

    static inline GLuint count = 0;

    GLuint binding = count++;

    std::vector<float> data;

    GLuint id = 0;

    std::vector<GLuint> subscribers;

    struct Definition {

        std::vector<Component*> components;
        std::vector<Component::Member*> members;

        int quantity;

        Definition() {}

        Definition(std::vector<Component*> components, int quantity = 1) : components(components), quantity(quantity) {

            for (auto &c:components) for (auto &m:c->members) members.push_back(&m);

        }

    };

    Definition definition;

    const char* name;

    ~UBO();
    
    UBO(const char* name, std::vector<Component*> components = {}, size_t quantity = 1, std::vector<GLuint> subscribers = {});

    void destroy();

    void resize(size_t size);

    void create(const char* name, size_t size, std::vector<GLuint> &subscribers);

    void link(GLuint shader);

    void update();

    void update(GLvoid* data, size_t size, GLuint offset = 0);

};


#endif // UBO_H
