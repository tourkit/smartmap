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

    Buffer::Object *framebuffers, *matrices;
    Buffer::Object::Entry specs;
    int sequid = 1000;

    ShaderProgram *basicshader;

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

        while (!glfwWindowShouldClose(window.id)) window.render([](){
            
            auto &engine = Engine::getInstance();

            engine.fb->clear();
            
            engine.dynamic_ubo.upload();

            engine.stack.run();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0,0,engine.window.width,engine.window.height);
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT);

            engine.fb->texture->bind();
            engine.basicshader->use();
            engine.quad->draw();


                engine.gui.draw();

            
            engine.specs.set<uint64_t>(0,engine.sequid--);
            if (!engine.sequid) engine.sequid = 1000;
      

        });

    };

private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

};

static inline  Engine &engine = Engine::getInstance();