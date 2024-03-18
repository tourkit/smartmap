#include "ubo.hpp"

#include "shader.hpp"
#include "struct.hpp"
#include "log.hpp"
#include <GL/gl3w.h>

UBO::~UBO() { destroy(); }

UBO::UBO(std::string name, std::vector<ShaderProgram*> subscribers) { 
    
    create(name,subscribers);

    binding = binding_count++;
    if (binding > 100) PLOGW << "MAX_UBO might soon be reached";// can do better ^^
     
    //  for (auto shader:subscribers) link(shader);

} 

Struct& UBO::add(Struct& s) {

        Struct::add(s);

        update();
        upload();

        return *this;
        
}

void UBO::destroy() { if (id<0) {glDeleteBuffers(1, &id); id = -1;} } // delete name; ?

void UBO::create(std::string name, std::vector<ShaderProgram*> subscribers) {

    this->name = name;
    this->subscribers = subscribers;

    // update();

}

void UBO::update() { // on Buffer change

    destroy();

    if (!data.size()) return;

    glGenBuffers(1, &id);

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, data.size(), NULL, GL_DYNAMIC_COPY);

    for (auto shader:subscribers) { // need link after resize ?

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glUniformBlockBinding(shader->id, glGetUniformBlockIndex(shader->id, name.c_str()), binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);

        // shader update

    }

}

void UBO::upload(){ upload(&data[0], data.size()); }

void UBO::upload(void* data, size_t size, uint32_t offset){

    // PLOGV << name << ": " << size;
    
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data); 
    
}