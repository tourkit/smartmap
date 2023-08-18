#pragma once

#include "pch.hpp"

#include "window.hpp"
#include "gui.hpp"
#include "ubo.hpp"

#include "vbo.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"


struct Engine {

    Window window;

    UBO dynamic_ubo, static_ubo;

    FrameBuffer fb;

    GUI gui;

    struct Stack {

        struct Stackable { virtual void run() {} };

        struct DrawCall : Stackable {

            VBO* vbo;
            ShaderProgram *shader;
            Texture *texture = nullptr;
            FrameBuffer *fb = &Engine::getInstance().fb;

            void run() override { 

                if (texture) texture->bind();
                shader->use();
                vbo->draw();

            }

        };

        struct Action : Stackable {

            std::function<void()> callback;

            void run() override { callback(); }

        };

        std::vector<Stackable> list;

        void run() { for (auto item : list) item.run(); }

    } stack;

    static Engine& getInstance() { static Engine instance;  return instance; }

    static void Run() {

        auto &window = getInstance().window;

        while (!glfwWindowShouldClose(window.id)) window.render([](){
            
            auto &engine = Engine::getInstance();

            engine.stack.run();

            engine.gui.newframe();

            engine.gui.render();

        });

    };

private:

    Engine(uint16_t width = 800, uint16_t height = 600) 

        : window(width,height), dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo"), fb(0,width,height), gui(window.id) {

        Component::init();

    }

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

};

static inline  Engine &engine = Engine::getInstance();