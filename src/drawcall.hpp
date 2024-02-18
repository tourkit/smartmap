#pragma once

#include "vbo.hpp"
#include "shader.hpp"

struct UBO;

struct DrawCall {

    DrawCall();

    VBO vbo;

    ShaderProgram shader;

    void update();

    void run();

    std::string layout(UBO* ubo);
};
