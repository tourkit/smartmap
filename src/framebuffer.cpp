#include "framebuffer.hpp"
#include "texture.hpp"
#include "engine.hpp"

FrameBuffer::~FrameBuffer() {

    destroy();

}

FrameBuffer::FrameBuffer(GLuint id, GLuint width, GLuint height) : id(id), width(width), height(height) {

    // auto n = Engine::getInstance().framebuffers->create();
    // n.set<uint32_t>(0,width);
    // n.set<uint32_t>(1,height);

}
void FrameBuffer::create(GLuint width, GLuint height){

    if (!destroy()) attachments++;

    this->width = width;
    this->height = height;

    texture = new Texture(width,height, 0,1,GL_RGB8 );

    glGenFramebuffers(1, &id);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(attachments), GL_TEXTURE_2D, texture->id, 0);

    std::vector<GLuint> drawBuffers;
    for (size_t i = 0; i < attachments+1; i++) drawBuffers.push_back( GL_COLOR_ATTACHMENT0+i);
    glDrawBuffers(attachments+1, &drawBuffers[0]);

}

FrameBuffer::FrameBuffer(GLuint width, GLuint height) {

    if (!width) width = Engine::getInstance().window.width;
    if (!height) height = Engine::getInstance().window.height;

    create(width, height);

}


bool FrameBuffer::destroy() {

    bool verif = false;

    if (id) { glDeleteFramebuffers(1, &id); verif = true; }

    if (texture) { delete texture; }

    return verif;

 }

void FrameBuffer::bind() {  glBindFramebuffer(GL_FRAMEBUFFER, id); glViewport(0,0,width,height); }

void FrameBuffer::clear(GLfloat r, GLfloat  g, GLfloat b, GLfloat a) {

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0,0,width,height);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

}

void FrameBuffer::read(unsigned char* data, GLenum format, GLuint width, GLuint height, GLuint x, GLuint y) {

    if (!width) width = this->width;

    if (!height) height = this->height;

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    glReadPixels(x, y, width, height, format, GL_UNSIGNED_BYTE, data);

    // need to FLIP image

    // for(int line = 0; line != h/2; ++line) {
    //     std::swap_ranges(
    //             local_data.begin() + 3 * w * line,
    //             local_data.begin() + 3 * w * (line+1),
    //             local_data.begin() + 3 * w * (h-line-1));
    // }


}
