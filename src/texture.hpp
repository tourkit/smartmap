#ifndef TEXTURE_H
#define TEXTURE_H	
	
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <string>

struct Texture {

    GLuint id, width, height,mipmaps = 1;

    GLenum format;

    Texture(GLuint width, GLuint height, GLenum format = GL_RGBA8, void* data = nullptr);

    void upload(void* data, GLuint width = 0, GLuint height = 0, GLuint offset_x = 0, GLuint offset_y = 0);

    void upload(std::string src, GLuint offset_x = 0, GLuint offset_y = 0);

    void copy(const Texture& texture, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);

    void bind();

    // operator GLuint();

};



#endif