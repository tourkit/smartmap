#pragma once

#include "pch.hpp"

#include "engine.hpp"

#include "vbo.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"

struct Stack {

    struct Stackable { virtual void run() ; };

    struct DrawCall : Stackable {

        VBO* vbo;
        ShaderProgram *shader;
        Texture *texture;
        FrameBuffer *fb;

        DrawCall(VBO* vbo, ShaderProgram *shader, Texture *texture = nullptr)  ;

        void run() override;

    };

    struct Action : Stackable {

        Action(std::function<void()> callback);

        std::function<void()> callback;

        void run() override;

    };

    std::vector<Stackable*> list;

    void run();

};