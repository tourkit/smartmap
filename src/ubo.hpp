#pragma once

#include <vector>

#include "buffer.hpp"
#include "component.hpp"

struct ShaderProgram;

struct UBO : Buffer {

    std::string name;

    uint32_t id = -1;

    uint32_t binding;

    bool loaded = false;

    static inline uint32_t binding_count = 0;

    static inline Components &comps = Components::getInstance();

    std::vector<ShaderProgram*> subscribers;

    ~UBO();
    
    UBO(std::string name = "ubo", std::vector<ShaderProgram*> subscribers = {});

    void destroy();

    void create(std::string name, std::vector<ShaderProgram*> subscribers);

    void update() override;

    void upload() override;

    void upload(void* data, size_t size, uint32_t offset = 0);
    
    // static void toJSON();
    
    // static void fromJSON();

};