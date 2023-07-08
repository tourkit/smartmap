#include "framebuffer.hpp"  
#include "texture.hpp"  

FrameBuffer::~FrameBuffer() { if (id) glDeleteFramebuffers(1, &id); }

FrameBuffer::FrameBuffer(GLuint id, GLuint width, GLuint height) : id(id), width(width), height(height) { }

FrameBuffer::FrameBuffer(Texture *tex) {  glGenFramebuffers(1, &id); 

    width = tex->width;
    height = tex->height;

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(attachments++), GL_TEXTURE_2D, tex->id, 0); 

    std::vector<GLuint> drawBuffers;
    for (size_t i = 0; i < attachments+1; i++) drawBuffers.push_back( GL_COLOR_ATTACHMENT0+i);
    glDrawBuffers(attachments+1, &drawBuffers[0]);
 
}

void FrameBuffer::bind() { glViewport(0,0,width,height); glBindFramebuffer(GL_FRAMEBUFFER, id);}

void FrameBuffer::clear(GLfloat r, GLfloat  g, GLfloat b, GLfloat a) {
    
    glViewport(0,0,width,height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

}

std::vector<unsigned char> FrameBuffer::read(GLuint width, GLuint height, GLuint x, GLuint y, GLenum format) {

    if (!width) width = this->width;
    if (!height) height = this->height;

    std::vector<unsigned char> data;

    data.resize(width*height*3);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    
    glReadPixels(x, y, width, height, format, GL_UNSIGNED_BYTE, &data[0]);

    return data;
 
}
