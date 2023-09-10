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
    
    GUI gui;

    UBO dynamic_ubo, static_ubo;

    FrameBuffer fb;

    VBO *quad;

    struct Stack {

        struct Stackable { virtual void run() {} };

        struct DrawCall : Stackable {

            VBO* vbo;
            ShaderProgram *shader;
            Texture *texture = nullptr;
            FrameBuffer *fb = &Engine::getInstance().fb; 

            DrawCall(VBO* vbo, ShaderProgram *shader) : vbo(vbo) , shader(shader) {}

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
        window.max_fps = 20;

        while (!glfwWindowShouldClose(window.id)) window.render([](){
            
            auto &engine = Engine::getInstance();
            
            engine.dynamic_ubo.update();

            engine.quad->draw();

            // engine.stack.run();

            engine.gui.draw();

        });

    };

private:

    Engine(uint16_t width = 800, uint16_t height = 600) 

        : window(width,height), dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo"), fb(0,width,height), gui(window.id) {

            window.setPos(2560,0);
            quad = new VBO("quad.obj",width,height);

    }

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

};

static inline  Engine &engine = Engine::getInstance();