#pragma once
	
#include <vector>
#include <string>

#include <GL/gl3w.h>

struct Texture {

    uint32_t id = 0, width, height, mipmaps = 1, unit = 0;

    GLenum informat = GL_RGB8;
    GLenum outformat = GL_RGB; 

    Texture(uint32_t width = 1, uint32_t height = 1, uint32_t unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(void* data, uint32_t width, uint32_t height, uint32_t unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    Texture(std::string path);
    
    ~Texture();

    void create(uint32_t width, uint32_t height, uint32_t unit, int mipmaps, GLenum informat, GLenum outformat);

    void write(void* data, uint32_t width, uint32_t height, uint32_t offset_x = 0 , uint32_t offset_y = 0, uint32_t unit = 0, int mipmaps = 1, GLenum informat = GL_RGB8, GLenum outformat = GL_RGB);

    void addImage(std::string path, uint32_t offset_x = 0, uint32_t offset_y= 0);
    void addChar(const char* chr,  int size = 200, uint32_t offset_x = 0, uint32_t offset_y= 0);

    void read(const Texture *texture, uint32_t offset_x = 0, uint32_t offset_y = 0);

    void bind(int unit);
    void bind();
    
    void destroy();

    void clear();
    
    operator uint32_t();

};