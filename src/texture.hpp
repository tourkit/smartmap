#pragma once

#include <vector>
#include <string>

#include <GL/gl3w.h>

struct Texture {

    GLuint id = 0, width, height, mipmaps = 1, unit = 0;

    GLenum informat = GL_RGB8;
    GLenum outformat = GL_RGB;

    std::string sampler_name;

    Texture(GLuint width = 1, GLuint height = 1, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(void* data, GLuint width, GLuint height, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(std::string path);

    ~Texture();

    void create(GLuint width, GLuint height);
    void create(GLuint width, GLuint height, GLuint unit, int mipmaps, GLenum informat, GLenum outformat);

    void write(void* data, GLuint width, GLuint height, GLuint offset_x = 0 , GLuint offset_y = 0, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    void addImage(std::string path, GLuint offset_x = 0, GLuint offset_y= 0);
    void addChar(const char* chr,  int size = 200, GLuint offset_x = 0, GLuint offset_y= 0);

    void read(const Texture *texture, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);
    void bind();

    void destroy();
    void reset();
    void clear();

    operator GLuint();

};
