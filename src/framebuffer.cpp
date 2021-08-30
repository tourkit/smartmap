#include "framebuffer.hpp"  
#include "window.hpp"  
#include "texture.hpp"  
	
extern GL::Window* window;
FrameBuffer::FrameBuffer(GLuint id) : id(0), width(window->width), height(window->width) { }
FrameBuffer::FrameBuffer() { glGenFramebuffers(1, &id); }
FrameBuffer::FrameBuffer(const Texture& tex) : FrameBuffer() { attach(tex); }
FrameBuffer::~FrameBuffer() { if (id) glDeleteFramebuffers(1, &id); }
void FrameBuffer::bind() { glViewport(0,0,width,height); glBindFramebuffer(GL_FRAMEBUFFER, id);}
void FrameBuffer::clear(GLfloat r, GLfloat  g, GLfloat b, GLfloat a) {
    
    glViewport(0,0,width,height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

}
void FrameBuffer::attach(const Texture& tex) {  

    width = tex.width;
    height = tex.height;

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(attachments++), GL_TEXTURE_2D, tex.id, 0); 

    std::vector<GLuint> drawBuffers;
    for (size_t i = 0; i < attachments+1; i++) drawBuffers.push_back( GL_COLOR_ATTACHMENT0+i);
    glDrawBuffers(attachments+1, &drawBuffers[0]);

}
