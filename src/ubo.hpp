#pragma once

#include "member.hpp"

struct ShaderProgram;

struct UBO : Member {

    uint32_t id = -1;

    int binding;

    bool loaded = false;

    static inline uint32_t binding_count = 0;

    ~UBO();
    
    UBO(std::string name = "ubo");

    void destroy();

    void create();

    void reset();

    void resize(uint32_t size);

    void update_pv() override;

    void upload() override;

    void upload(void* data, size_t size, uint32_t offset = 0);

    void bind(uint32_t shader);

};