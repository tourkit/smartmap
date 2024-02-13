#pragma once

#include <vector>

#include "buffer.hpp"
#include "component.hpp"

struct ShaderProgram;
struct UBO : BufferOwner {

    std::string name;

    uint32_t id = -1;

    uint32_t binding;

    static inline uint32_t binding_count = 0;

    static inline Components &comps = Components::getInstance();

    std::vector<ShaderProgram*> subscribers;

    Buffer buffer;

    ~UBO();
    
    UBO(std::string name = "ubo", std::vector<ShaderProgram*> subscribers = {});

    void destroy();

    void update() override;

    void upload() override;

    void upload(void* data, size_t size, uint32_t offset = 0);
    
    // static void toJSON();
    
    // static void fromJSON();

};