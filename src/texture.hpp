#ifndef TEXTURE_H
#define TEXTURE_H	
	
#include "globals.hpp"


struct Texture {

    static inline std::vector<Texture*> pool;

    // static inline uint8_t white[4] = {255,255,0,255};

    GLuint id = 0, width, height,mipmaps = 1, unit = 0;

    GLenum format = GL_RGB8;

    std::string path;

    Texture();
    Texture(void* data, GLuint width, GLuint height);
    Texture(std::string src);
    
    ~Texture();

    void create(void* data, GLuint width, GLuint height, GLuint offset_x = 0, GLuint offset_y = 0, GLuint unit= 0);
    void update(void* data, GLuint width, GLuint height, GLuint offset_x = 0, GLuint offset_y = 0);

    void create(std::string src, GLuint offset_x = 0, GLuint offset_y = 0);

    void copy(const Texture *texture, GLuint offset_x = 0, GLuint offset_y = 0);

    void bind(int unit);

    void reset();
    void destroy();
    void bind();

    operator GLuint();

};



#endif