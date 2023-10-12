#include "engine.hpp"
#include "stack.hpp"


void Stack::Stackable::run() {  } 

Stack::DrawCall::DrawCall(VBO* vbo, ShaderProgram *shader, Texture *texture) : vbo(vbo) , shader(shader), texture(texture) { FrameBuffer *fb = &Engine::getInstance().fb; }

void Stack::DrawCall::run() { 

    if (texture) texture->bind();
    shader->use();
    vbo->draw();

}
Stack::Action::Action(std::function<void()> callback) : callback(callback) {}


void Stack::Action::run() { callback(); }


void Stack::run() { for (auto item : list) item->run();  }

