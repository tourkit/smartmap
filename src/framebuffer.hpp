#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
	
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

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