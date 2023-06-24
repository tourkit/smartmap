#ifndef TEXTURE_H
#define TEXTURE_H	
	
#include "globals.hpp"


struct Texture {

    // static inline uint8_t white[4] = {255,255,0,255};

    GLuint id, width, height,mipmaps = 1;

    GLenum format;


    Texture(GLuint width, GLuint height, void* data, GLenum format = GL_RGB8);

    void upload(void* data, GLuint width = 0, GLuint height = 0, GLuint offset_x = 0, GLuint offset_y = 0);

    void upload(std::string src, GLuint offset_x = 0, GLuint offset_y = 0);

    void copy(const Texture& texture, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);

    void bind();

    operator GLuint();

};



#endif