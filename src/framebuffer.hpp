#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H


#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "texture.hpp"


struct FrameBuffer {

    uint32_t id = 0, width = 0, height = 0, attachments = 0;

    Texture texture;

    float clear_color[4] = {0,0,0,0};

    static inline std::vector<FrameBuffer*> pool;

    FrameBuffer(GLuint id, GLuint width, GLuint height);

    FrameBuffer(GLuint width, GLuint height);

    void create(GLuint width, GLuint height);

    ~FrameBuffer();

    bool destroy();

    void bind();

    void clear();

    void clear(GLfloat r, GLfloat  g, GLfloat b, GLfloat a);

    void read(unsigned char* data, GLenum format = GL_RGB, GLuint width = 0, GLuint height = 0, GLuint x = 0, GLuint y = 0);

};
#endif
