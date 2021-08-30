#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
	
#include "globals.hpp"

struct Texture;
	
struct FrameBuffer {

    GLuint id, width, height, attachments = 0;
    FrameBuffer(GLuint id);
    FrameBuffer();
    FrameBuffer(const Texture& tex);
    ~FrameBuffer();
    void bind();
    void clear(GLfloat r = 0, GLfloat  g = 0, GLfloat b = 0, GLfloat a = 0);
    void attach(const Texture& tex);

};
#endif