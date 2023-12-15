#include "engine.hpp"
#include "stack.hpp"
///////////////////// STACK




///////////////////// CUE

Stack::Cue::~Cue() { if (owner) owner->remove(this); }

Stack::Cue* Stack::Cue::prepend(Cue* cue) {

    return owner->add(cue, std::distance(owner->list.begin(), std::find(owner->list.begin(), owner->list.end(), this)));
}

Stack::Cue* Stack::Cue::append(Cue* cue) {

    return owner->add(cue, std::distance(owner->list.begin(), std::find(owner->list.begin(), owner->list.end(), this))+1);

}

void  Stack::Cue::up() { 
    
    if (!owner) return;


    auto it = std::find(owner->list.begin(), owner->list.end(), this);
    int index = std::distance(owner->list.begin(), it);

    if(index < 1) return; 

    owner->list.erase(it);
    owner->list.insert(owner->list.begin() + index - 1, this);

}

void  Stack::Cue::down() {

    if (!owner) return;
    
    auto it = std::find(owner->list.begin(), owner->list.end(), this);
    int index = std::distance(owner->list.begin(), it);  

    if(index > owner->list.size()-2) return;

    owner->list.erase(it);
    owner->list.insert(owner->list.begin() + index + 1, this); 

}

void Stack::Cue::run() {  } 

///////////////////// STACKCUE

Stack::StackCue::StackCue(Stack *stack, std::string name) : stack(stack) { 
  

}

void Stack::StackCue::run() { 

    stack->run();

}
///////////////////// CLEARCUE

Stack::ClearCall::ClearCall(FrameBuffer *fb, std::string name)  { 
  
    this->fb = fb;

}
///////////////////// DRAWCUE

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

///////////////////// ActionCue

Stack::Action::Action(std::function<void()> callback, std::string name) 

    : callback(callback) {

    this->name = name;

}


void Stack::Action::run() { callback(); }


void Stack::run() { 

    for (auto cue : list) if (cue->active) cue->run(); 
    for (auto child : childrens) child->run(); 
    
}

