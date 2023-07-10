#include "ubo.hpp"



UBO::~UBO() { destroy(); }

UBO::UBO() { }

UBO::UBO(const char* name, size_t size, std::vector<GLuint> subscribers) { create(name,size,subscribers); for (auto shader:subscribers) link(shader); } 

void UBO::destroy() { data.resize(0); subscribers.resize(0);  if (id) glDeleteBuffers(1, &id); } // delete name; ?

void UBO::resize(size_t size) { auto t_name = name; auto t_subscribers = subscribers; destroy(); create(t_name, size, t_subscribers); }

void UBO::create(const char* name, size_t size, std::vector<GLuint> &subscribers) {

    destroy();

    this->name = name;

    this->subscribers = subscribers;

    data.resize(size);

        // can do better ^^
    if (binding > 100) std::cout << "MAX_UBO might soon be reached";

    glGenBuffers(1, &id);

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, 4*data.size(), NULL, GL_DYNAMIC_COPY);

    std::cout << "RTFM /!\\ put good bindings in shader !! layout(std140, binding = " << binding << ") uniform " << name << " { size:" << data.size() << " };" << std::endl;
    
}

void UBO::link(GLuint shader) {

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, name), binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
}


void UBO::update(){ update(&data[0], 4*data.size()); }

void UBO::update(GLvoid* data, size_t size, GLuint offset){

    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data); 

}