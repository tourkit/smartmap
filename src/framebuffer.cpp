#include "framebuffer.hpp"
#include "texture.hpp"
#include "utils.hpp"

FrameBuffer::~FrameBuffer() {

    REMOVE<FrameBuffer*>(pool, this);

    destroy();

}

FrameBuffer::FrameBuffer(GLuint id, GLuint width, GLuint height) : id(id) {

    // n.set<uint32_t>(0,width);
    // n.set<uint32_t>(1,height);

    ADD_UNIQUE<FrameBuffer*>(pool, this);

}

FrameBuffer::FrameBuffer(GLuint width, GLuint height) : texture(width, height) {

    create(width, height);

    ADD_UNIQUE<FrameBuffer*>(pool, this);
}


void FrameBuffer::create(GLuint width, GLuint height){

    texture.informat = GL_RGB8;

    if (!texture.create(width,height) && id)
        return;

    destroy();


    glGenFramebuffers(1, &id);

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(attachments), GL_TEXTURE_2D, texture.id, 0);

    std::vector<GLuint> drawBuffers;
    for (size_t i = 0; i < attachments+1; i++) drawBuffers.push_back( GL_COLOR_ATTACHMENT0+i);
    glDrawBuffers(attachments+1, &drawBuffers[0]);

}



bool FrameBuffer::destroy() {

    bool verif = false;

    if (id) { glDeleteFramebuffers(1, &id); verif = true; }

    // if (texture) { delete texture; }

    return verif;

 }

void FrameBuffer::bind() {  glBindFramebuffer(GL_FRAMEBUFFER, id); glViewport(0,0,texture.width,texture.height); }

void FrameBuffer::clear() {

    clear(clear_color[0],clear_color[1],clear_color[2],clear_color[3]);

}

void FrameBuffer::clear(GLfloat r, GLfloat  g, GLfloat b, GLfloat a) {

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0,0,texture.width,texture.height);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);

}

void FrameBuffer::read(unsigned char* data, GLenum format, GLuint width, GLuint height, GLuint x, GLuint y) {

    if (!width) width = texture.width;

    if (!height) height = texture.height;

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
