#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"

#include <vector>
#include <cstring>

struct UBO;
struct FrameBuffer;

struct DrawCall {

    DrawCall();

    VBO vbo;

    ShaderProgram shader;

    void update();

    virtual void draw();

};


struct Layer : DrawCall {

    static inline File *quad = new File("assets/models/quad.obj");

    FrameBuffer fb;

    void draw() override;

    Layer(uint16_t width, uint16_t height);

    Layer();

    Texture* feedback = nullptr;

};
