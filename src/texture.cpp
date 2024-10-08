#include "texture.hpp"
#include "image.hpp"
#include "utils.hpp"
// #include "freetype.hpp"


Texture::Texture(GLuint width, GLuint height, GLuint unit, int mipmaps, GLenum informat, GLenum outformat) 
:
    unit(unit),
    informat(informat),
    outformat(outformat),
    mipmaps(mipmaps)

{
     
    create(width,height);

    ADD_UNIQUE<Texture*>(pool, this);

}

Texture::Texture(void* data, GLuint width, GLuint height, GLuint unit, int mipmaps, GLenum informat, GLenum outformat) :

    Texture(width, height, unit, mipmaps, informat, outformat) {

    write(data, width, height, 0, 0);

}

Texture::Texture(std::string path)   {

    addImage(path);

    ADD_UNIQUE<Texture*>(pool, this);

 }

Texture::~Texture() { 
    
    destroy(); 

    REMOVE<Texture*>(pool, this);

}

void Texture::reset() { create( width, height); }

bool Texture::create(GLuint width, GLuint height) {
    
    if (this->width == width && this->height == height)
        return false;

    destroy();

    static uint32_t p_max = 0;
    if (!p_max) {
        static int32_t int_p_max = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &int_p_max);
        p_max = int_p_max;
    }

    if (!width) {
        PLOGE << "width cant be <1px";
        width = 1;
    }
    else if (width > p_max) {
        PLOGE << "width GL_MAX_TEXTURE_SIZE " << p_max << "px";
        width = p_max;
    }
    if (!height) {
        PLOGE << "height cant be <1px";
        height = 1;
    }
    else if (height > p_max) {
        PLOGE << "height GL_MAX_TEXTURE_SIZE " << p_max << "px";
        height = p_max;
    }
    
    this->width = width;
    this->height = height;

    glGenTextures(1, &id);

    PLOGV << width << "x" << height << " - id=" << id << " " << ", unit=" << unit << ", mipmaps=" << mipmaps;

    glActiveTexture(GL_TEXTURE0+unit);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexStorage2D(GL_TEXTURE_2D, mipmaps, informat, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glActiveTexture(GL_TEXTURE0);

    return true;

}

void Texture::addChar(const char* chr,  int size, GLuint offset_x, GLuint offset_y) {

    // Freetype fr(chr, size);

    // write(fr.buffer, fr.width, fr.height,offset_x, offset_y,0,1,GL_RGB8,GL_RED);

}

void Texture::addImage(std::string path, GLuint offset_x, GLuint offset_y) {

    Image img(path);

    if (img.width) {
    
        if (width< img.width+offset_x || height < img.height+offset_y) 
            create(img.width, img.height);

        write(&img.data[0], img.width, img.height,offset_x, offset_y);
    
    }

}

void Texture::destroy() {

    glActiveTexture(GL_TEXTURE0+unit);

    if (id) glDeleteTextures(1, &id);

    glActiveTexture(GL_TEXTURE0);

}

void Texture::bind(int unit) { 
    // PLOGW << "bind text - id:" << id << " unit:" << unit << " ,sampler_name: " << sampler_name;
    glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, id); glActiveTexture(GL_TEXTURE0);  
}

void Texture::bind() { bind(unit); }

void Texture::write(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y) {

    if (!width)
        width = this->width;
    if (!height)
        height = this->height;

    glActiveTexture(GL_TEXTURE0+unit);

    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,offset_x,offset_y,width,height,outformat,GL_UNSIGNED_BYTE,data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);

}

void Texture::read(const Texture* texture, GLuint offset_x, GLuint offset_y) {

    // PLOGW << "read tex " << sampler_name << " width: " << texture->width << " height: " << texture->height << " offset_x: " << offset_x << " offset_y: " << offset_y;

    glCopyImageSubData(texture->id, GL_TEXTURE_2D, 0, 0,0, 0, id, GL_TEXTURE_2D, 0, offset_x, offset_y, 0, texture->width,texture->height, 1);

    // GL_ERROR();

}

void Texture::clear() {

    std::vector<char> black;

    black.resize(width*height*4);

    write(&black[0], width, height, 0,0);

}
