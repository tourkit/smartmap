#ifndef TEXTURE_H
#define TEXTURE_H	
	
#include "globals.hpp"


struct Texture {

    static inline std::vector<Texture*> pool;

    GLuint id = 0, width, height, mipmaps = 1, unit = 0;

    GLenum format = GL_RGB8;

    std::string path;

    Texture();
    Texture(void* data, GLuint width, GLuint height, GLenum format = GL_RGB8);
    Texture(std::string src);
    
    ~Texture();

    void create(std::string src, GLuint offset_x = 0, GLuint offset_y = 0);
    void create(void* data, GLuint width, GLuint height, GLuint offset_x = 0, GLuint offset_y = 0, GLuint unit= 0);

    void read(const Texture *texture, GLuint offset_x = 0, GLuint offset_y = 0);
    void write(void* data, GLuint width, GLuint height, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);
    void bind();

    void reset();
    void destroy();
    
    operator GLuint();

};



#endif