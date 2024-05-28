#include "ubo.hpp"

#include "shader.hpp"
#include "struct.hpp"
#include "log.hpp"
#include <GL/gl3w.h>

UBO::~UBO() { destroy(); }

UBO::UBO(std::string name) : Buffer(name) {

    striding(true);

    binding = binding_count++;

    if (binding > 100) PLOGE << "MAX_UBO might soon be reached";// can do better ^^

    create();

}

void UBO::destroy() { if (id<0) {glDeleteBuffers(1, &id); id = -1;} }

void UBO::resize(uint32_t size) {

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_COPY);

}

void UBO::create() {

    glGenBuffers(1, &id);

    resize(footprint_all());

}

void UBO:: bind(ShaderProgram* shader) {

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glUniformBlockBinding(shader->id, glGetUniformBlockIndex(shader->id, name().c_str()), binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);

}

void UBO::update() { Buffer::update(); resize(footprint_all()); } // acho q melhor nao ter upload() la. obrigado

void UBO::reset() {

    destroy();

    create();

    upload();

}

void UBO::upload(){ upload(data.data(), data.size()); }

void UBO::upload(void* data, size_t size, uint32_t offset){

    std::string str;
    for (int i = 0 ; i < this->data.size(); i++) str+= std::to_string(*(((uint8_t*)data)+i)) + " ";
    if (binding /*aka statubo*/) PLOGV << name() << " " << binding << ": " << size << " - " << str;

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

}
