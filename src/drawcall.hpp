#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"

struct UBO;
struct FrameBuffer;

struct DrawCall {

    DrawCall();

    VBO vbo;

    ShaderProgram shader;

    void update();

    virtual void run();

    std::string layout(UBO* ubo);
};


struct Layer : DrawCall {

    FrameBuffer fb;

    void run() override {

        fb.bind();

        DrawCall::run();
    }
    
    Layer(uint16_t width, uint16_t height);

    Layer();
    
};