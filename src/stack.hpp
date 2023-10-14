#pragma once

#include "pch.hpp"

#include "engine.hpp"

#include "vbo.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"


static void Draw2D(VBO* vbo, ShaderProgram *shader, FrameBuffer *fb, Texture *texture = nullptr) {

    fb->bind();
    if (texture) texture->bind();
    shader->use();
    vbo->draw();

}

struct Stack {

    struct Cue { 
        
        std::string name = "Cue"; 
        bool active = true;
        virtual void run() ; 
        
    };

    struct ClearCall : Cue {

        FrameBuffer *fb;

        ClearCall(FrameBuffer *fb = nullptr, std::string name = "ClearCall")  ;

        void run() override;
    };
    
    struct DrawCall : Cue {

        VBO* vbo;
        ShaderProgram *shader;
        Texture *texture;
        FrameBuffer *fb;

        DrawCall(VBO* vbo, ShaderProgram *shader, Texture *texture = nullptr, FrameBuffer *fb = nullptr, std::string name = "DrawCall")  ;

        void run() override;

    };
    
    struct Action : Cue {

        Action(std::function<void()> callback, std::string name = "Action");

        std::function<void()> callback;

        void run() override;

    };

    std::vector<Cue*> list;

    void run();

};