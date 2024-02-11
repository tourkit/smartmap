#pragma once

#include "vbo.hpp"
#include "shader.hpp"

struct DrawCall {

    VBO vbo;

    ShaderProgram shader;

    void update();

    void run();
};
