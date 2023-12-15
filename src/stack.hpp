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

    Stack* parent = nullptr;
    std::vector<Stack*> childrens;
    std::string name = "Stack"; 
    

    struct Cue { 
        
        std::string name = "Cue"; 
        Stack* owner = nullptr;
        bool active = true;

        Cue* prepend(Cue* cue);
        Cue* append(Cue* cue);

        ~Cue();

        void up();
        void down();

        virtual void run() ; 
        
    };

    std::vector<Cue*> list;
    
    struct StackCue : Cue { 
        
        std::string name = "Stack"; 
        Stack *stack;

        StackCue(Stack *stack, std::string name = "ClearCall")  ;

        void run() override;
        
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

    Cue* add(Cue* cue, int index = -1) { 

        cue->owner = this; 
        
        auto it = list.end();
        if (index >-1) it = list.begin()+index;
        list.insert(it, cue);

        return cue;

    }

    void remove(Cue* cue) { 

        cue->owner = nullptr;  

        list.erase(std::find(list.begin(), list.end(), cue)); 
        
    }

    void up() { 
        
        if (!parent) return;

        auto it = std::find(parent->childrens.begin(), parent->childrens.end(), this);
        int index = std::distance(parent->childrens.begin(), it);

        if(index<1) return; 

        parent->childrens.erase(it);
        parent->childrens.insert(parent->childrens.begin() + index - 1, this);
    
    }

    void down() {

        if (!parent) return;
        
        auto it = std::find(parent->childrens.begin(), parent->childrens.end(), this);
        int index = std::distance(parent->childrens.begin(), it);  

        if(index > parent->childrens.size()-2) return;

        parent->childrens.erase(it);
        parent->childrens.insert(parent->childrens.begin() + index + 1, this); 

    }

    Stack* child(std::string name) { 

        for (auto c : childrens) if (c->name == name) return c;
        
        childrens.push_back(new Stack{});
        childrens.back()->name = name;        
        childrens.back()->parent = this;

        return childrens.back();

    }

    void run();

};