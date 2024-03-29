#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
	

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

struct Texture;
	
struct FrameBuffer {

    uint32_t id, width, height, attachments = 0;
    std::string name;
    Texture *texture = nullptr;
    FrameBuffer(GLuint id, GLuint width, GLuint height,std::string name="FrameBuffer");
    FrameBuffer(GLuint width = 0, GLuint height = 0,std::string name="FrameBuffer");
    ~FrameBuffer();
    void bind();
    void clear(GLfloat r = 0, GLfloat  g = 0, GLfloat b = 0, GLfloat a = 0);
    unsigned char* read(GLuint width = 0, GLuint height = 0, GLuint x = 0, GLuint y = 0, GLenum format = GL_RGB, unsigned char *data = nullptr);

};
#endif 