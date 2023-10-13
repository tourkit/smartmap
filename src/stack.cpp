#include "engine.hpp"
#include "stack.hpp"


void Stack::Cue::run() {  } 

Stack::ClearCall::ClearCall(FrameBuffer *fb, std::string name)  { 
  
    if (!fb) this->fb = &Engine::getInstance().fb; 
    else this->fb = fb;

}

Stack::DrawCall::DrawCall(VBO* vbo, ShaderProgram *shader, Texture *texture, FrameBuffer *fb, std::string name) 
 
    : vbo(vbo), shader(shader), texture(texture) { 
    
    this->name = name;

    if (!fb) this->fb = &Engine::getInstance().fb; 
    else this->fb = fb;

}

void Stack::ClearCall::run() { 

    // fb

}
void Stack::DrawCall::run() { 

    fb->bind();
    if (texture) texture->bind();
    shader->use();
    vbo->draw();

}
Stack::Action::Action(std::function<void()> callback, std::string name) 

    : callback(callback) {

    this->name = name;

}


void Stack::Action::run() { callback(); }


void Stack::run() { for (auto item : list) item->run();  }

