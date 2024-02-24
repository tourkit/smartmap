#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"

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

    Texture texture;

    void run() override {

        fb.bind();

        DrawCall::run();
    }
    
};