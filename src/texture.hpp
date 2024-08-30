#pragma once

#include <vector>
#include <string>

#include <GL/gl3w.h>

struct Texture {

    GLuint id = 0, width, height, mipmaps, unit;

    GLenum informat = GL_RGB8;
    GLenum outformat = GL_RGB;

    std::string sampler_name;

    Texture(GLuint width = 0, GLuint height = 0, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(void* data, GLuint width, GLuint height, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(std::string path);

    ~Texture();

    void create(GLuint width, GLuint height);

    void write(void* data, GLuint width = 0, GLuint height = 0, GLuint offset_x = 0 , GLuint offset_y = 0);

    void addImage(std::string path, GLuint offset_x = 0, GLuint offset_y= 0);

    void addChar(const char* chr,  int size = 200, GLuint offset_x = 0, GLuint offset_y= 0);

    void read(const Texture *texture, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);
    void bind();

    void destroy();
    void reset();
    void clear();

    operator GLuint();

    static inline std::vector<Texture*> pool;

};
