#ifndef TEXTURE_H
#define TEXTURE_H	
	
#include "globals.hpp"




struct Texture {

    static inline std::vector<Texture*> pool;

    GLuint id = 0, width, height, mipmaps = 1, unit = 0;

    GLenum informat = GL_RGB8;
    GLenum outformat = GL_RGB;

    Texture(GLuint width = 1, GLuint height = 1, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(void* data, GLuint width, GLuint height, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(std::string path);
    
    ~Texture();

    void write(void* data, GLuint width, GLuint height, GLuint offset_x = 0 , GLuint offset_y = 0, GLuint unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    void addImage(std::string path, GLuint offset_x = 0, GLuint offset_y= 0);
    void addChar(const char* chr,  int size = 200, GLuint offset_x = 0, GLuint offset_y= 0);

    void read(const Texture *texture, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);
    void bind();
    
    void destroy();
    
    operator GLuint();

};



#endif