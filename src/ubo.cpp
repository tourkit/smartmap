#include "ubo.hpp"

#include "member.hpp"
#include "shader.hpp"
#include <GL/gl3w.h>

UBO::~UBO() { destroy(); }

UBO::UBO(std::string name) : Member(name) {

    striding(true);

    buffering(true);

    binding = binding_count++;

    if (binding > 100) {PLOGE << "MAX_UBO might soon be reached";}// can do better ^^

    create();

}

void UBO::destroy() { 

    if (id<0) 
        glDeleteBuffers(1, &id); id = -1; 

}

void UBO::resize(uint32_t size) {

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_COPY);

}

void UBO::create() {

    glGenBuffers(1, &id);

    resize(footprint_all());

}

void UBO:: bind(uint32_t shader) {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, name().c_str()), binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);

}

void UBO::update() { 

    Member::update(); 
    
    resize(footprint_all()); 

} 

void UBO::reset() {

    destroy();

    create();

    upload();

}

void UBO::upload(){ upload(data(), footprint_all()); }

void UBO::upload(void* data, size_t size, uint32_t offset){

    // std::string str;
    // for (int i = footprint() ; i < footprint(); i++) str+= std::to_string(*(((uint8_t*)data)+i)) + " ";
    // if (name() == "dynamic_ubo") {PLOGW << name() << " " << id << " " << binding << ": " << size << " - " << str;}

    if (!size) return;
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

}
