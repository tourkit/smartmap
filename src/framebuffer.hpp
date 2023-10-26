#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
	
#include "pch.hpp"

struct Texture;
	
struct FrameBuffer {

    static inline std::vector<FrameBuffer*> pool;
    GLuint id, width, height, attachments = 0;
    std::string name;
    Texture *texture;
    FrameBuffer(GLuint id, GLuint width, GLuint height,std::string name="FrameBuffer");
    FrameBuffer(GLuint width, GLuint height,std::string name="FrameBuffer");
    FrameBuffer(Texture *tex,std::string name="FrameBufferTex");
    void link(Texture *tex);
    ~FrameBuffer();
    void bind();
    void clear(GLfloat r = 0, GLfloat  g = 0, GLfloat b = 0, GLfloat a = 0);
    unsigned char* read(GLuint width = 0, GLuint height = 0, GLuint x = 0, GLuint y = 0, GLenum format = GL_RGB, unsigned char *data = nullptr);

};
#endif 