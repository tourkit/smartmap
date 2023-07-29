#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
	
#include "globals.hpp"

struct Texture;
	
struct FrameBuffer {

    GLuint id, width, height, attachments = 0;
    FrameBuffer(GLuint id, GLuint width, GLuint height);
    FrameBuffer(Texture *tex);
    ~FrameBuffer();
    void bind();
    void clear(GLfloat r = 0, GLfloat  g = 0, GLfloat b = 0, GLfloat a = 0);
    unsigned char* read(GLuint width = 0, GLuint height = 0, GLuint x = 0, GLuint y = 0, GLenum format = GL_RGB, unsigned char *data = nullptr);

};
#endif 