#include "framebuffer.hpp"  
#include "texture.hpp"  
#include "engine.hpp"  

FrameBuffer::~FrameBuffer() { 
    
    if (id) {glDeleteFramebuffers(1, &id);} 

    if (texture) { delete texture; } 
    
}

FrameBuffer::FrameBuffer(GLuint id, GLuint width, GLuint height,std::string name) : id(id), width(width), height(height), name(name) { pool.push_back(this); 

    auto n = engine.framebuffers->create();
    n.set<uint32_t>(0,width);
    n.set<uint32_t>(1,height);

}

FrameBuffer::FrameBuffer(GLuint width, GLuint height,std::string name) : name(name) { 
    

    pool.push_back(this); 

    if (!width) width = Engine::getInstance().window.width;
    if (!height) height = Engine::getInstance().window.height;
    this->width = width;
    this->height = height;

    auto n = engine.framebuffers->create();
    n.set<uint32_t>(0,width);
    n.set<uint32_t>(1,height);

    texture = new Texture(width,height, 0,1,GL_RGB8 );

    glGenFramebuffers(1, &id); 

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(attachments++), GL_TEXTURE_2D, texture->id, 0); 

    std::vector<GLuint> drawBuffers;
    for (size_t i = 0; i < attachments+1; i++) drawBuffers.push_back( GL_COLOR_ATTACHMENT0+i);
    glDrawBuffers(attachments+1, &drawBuffers[0]); 
 
}


void FrameBuffer::bind() {  glBindFramebuffer(GL_FRAMEBUFFER, id); glViewport(0,0,width,height); }

void FrameBuffer::clear(GLfloat r, GLfloat  g, GLfloat b, GLfloat a) {

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0,0,width,height);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

}

unsigned char* FrameBuffer::read(GLuint width, GLuint height, GLuint x, GLuint y, GLenum format, unsigned char *data) {

    if (!width) width = this->width;
    if (!height) height = this->height;

    std::vector<unsigned char> local_data;

    if (!data) {
    
        local_data.resize(width*height*3);
        data = &data[0];

    }

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    
    
    glReadPixels(x, y, width, height, format, GL_UNSIGNED_BYTE, data);

    // need to FLIP image

    // for(int line = 0; line != h/2; ++line) {
    //     std::swap_ranges(
    //             local_data.begin() + 3 * w * line,
    //             local_data.begin() + 3 * w * (line+1),
    //             local_data.begin() + 3 * w * (h-line-1));
    // }


    
    return data;
 
}
