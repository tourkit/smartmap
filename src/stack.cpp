#include "engine.hpp"
#include "stack.hpp"


void Stack::Cue::run() {  } 

Stack::StackCue::StackCue(Stack *stack, std::string name) : stack(stack) { 
  

}

void Stack::StackCue::run() { 

    stack->run();

}

Stack::ClearCall::ClearCall(FrameBuffer *fb, std::string name)  { 
  
    this->fb = fb;

}

Stack::DrawCall::DrawCall(VBO* vbo, ShaderProgram *shader, Texture *texture, FrameBuffer *fb, std::string name) 
 
    : vbo(vbo), shader(shader), texture(texture) { 
    
    if (!vbo || !shader) active = false;
    
    this->name = name;

    this->fb = fb;

}
void Stack::DrawCall::run() { 

    if (!active || !shader->loaded) return;

    fb->bind();
    if (texture) texture->bind();
    shader->use();
    vbo->draw();

}

void Stack::ClearCall::run() { 

    // fb

}


Stack::Action::Action(std::function<void()> callback, std::string name) 

    : callback(callback) {

    this->name = name;

}


void Stack::Action::run() { callback(); }


void Stack::run() { 

    for (auto cue : list) if (cue->active) cue->run(); 
    for (auto child : childrens) child->run(); 
    
}

