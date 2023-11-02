#pragma once

#include "pch.hpp"

#include "window.hpp"
#include "ubo.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "stack.hpp"
#include "gui.hpp"

#include "widgets/buffer.hpp"
#include "widgets/textures.hpp"

struct Engine {

    Window window;

    UBO dynamic_ubo, static_ubo;

    Buffer::Object *framebuffers;

    FrameBuffer *fb;

    VBO *quad;

    Stack stack;
    
    GUI gui;

    BufferWidget bw;
    TexturesWidget tw;

    void init();

    static Engine& getInstance() { static Engine instance;  return instance; }

    static void Run() {

        auto &window = getInstance().window;
        window.max_fps = 20;

        while (!glfwWindowShouldClose(window.id)) window.render([](){
            
            auto &engine = Engine::getInstance();
            
            engine.dynamic_ubo.upload();

            engine.stack.run();

            engine.gui.draw();

        });

    };

private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

};

static inline  Engine &engine = Engine::getInstance();