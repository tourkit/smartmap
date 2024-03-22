#pragma once

#include <vector>

#include "buffer.hpp"

struct ShaderProgram;

struct UBO : Buffer {

    uint32_t id = -1;

    uint32_t binding;

    bool loaded = false;

    static inline uint32_t binding_count = 0;

    std::vector<ShaderProgram*> subscribers;

    ~UBO();
    
    UBO(std::string name = "ubo", std::vector<ShaderProgram*> subscribers = {});

    void destroy();

    void create();

    void reset();

    void update() override;

    void upload() override;

    void upload(void* data, size_t size, uint32_t offset = 0);

    Struct& add(Struct& s) override;

};