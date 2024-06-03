#include "texture.hpp"
#include "image.hpp"
#include "log.hpp"
// #include "freetype.hpp"


Texture::Texture(GLuint width, GLuint height, GLuint unit, int mipmaps, GLenum informat, GLenum outformat) {

    create(width,height,unit,mipmaps,informat,outformat);

}

void Texture::create(GLuint width, GLuint height, GLuint unit, int mipmaps, GLenum informat, GLenum outformat) {
    destroy();
    this->unit = unit;
    this->informat = informat;
    this->outformat = outformat;
    this->mipmaps = mipmaps;
    create( width, height);
};

void Texture::reset() { destroy();create( width, height); }
void Texture::create(GLuint width, GLuint height) {

    this->width = width;
    this->height = height;

    glGenTextures(1, &id);

    PLOGD << width << " x " << height << " - id=" << id << " " << ", unit=" << unit << ", mipmaps=" << mipmaps;

    glActiveTexture(GL_TEXTURE0+unit);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexStorage2D(GL_TEXTURE_2D, mipmaps, informat, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glActiveTexture(GL_TEXTURE0);

}

Texture::Texture(void* data, GLuint width, GLuint height, GLuint unit, int mipmaps, GLenum informat, GLenum outformat) {

        create(width,height,unit,mipmaps,informat,outformat);

        write(data, width, height, 0, 0, unit, 1, informat, outformat);

}

Texture::Texture(std::string path)   {

    Image img(path);

    if (!img.width) { PLOGE << "error in Texture creation" ; return; }

    create(img.width, img.height,0,1,GL_RGB8,GL_RGB);

    write(&img.

    data[0], img.width, img.height,0, 0,0,1,GL_RGB8,GL_RGB);

 }

void Texture::addChar(const char* chr,  int size, GLuint offset_x, GLuint offset_y) {

    // Freetype fr(chr, size);

    // write(fr.buffer, fr.width, fr.height,offset_x, offset_y,0,1,GL_RGB8,GL_RED);

}

void Texture::addImage(std::string path, GLuint offset_x, GLuint offset_y) {

    Image img(path);

    if (img.width) write(&img.data[0], img.width, img.height,offset_x, offset_y,0,1,GL_RGB8,GL_RGB);

}

Texture::~Texture() { destroy(); }

void Texture::destroy() {

    glActiveTexture(GL_TEXTURE0+unit);

    if (id) glDeleteTextures(1, &id);

}

void Texture::bind(int unit) { this->unit = unit; bind(); }

void Texture::bind() { glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, id); glActiveTexture(GL_TEXTURE0); }

Texture::operator GLuint() { return id; }

void Texture::write(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y, GLuint unit, int mipmaps, GLenum informat, GLenum outformat) {

    glActiveTexture(GL_TEXTURE0+unit);

    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,offset_x,offset_y,width,height,outformat,GL_UNSIGNED_BYTE,data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);

}

void Texture::read(const Texture* texture, GLuint offset_x, GLuint offset_y) {

    glCopyImageSubData(texture->id, GL_TEXTURE_2D, 0, 0,0, 0, id, GL_TEXTURE_2D, 0, offset_x, offset_y, 0, texture->width,texture->height, 1);

    // GL_ERROR();

}

void Texture::clear() {

    std::vector<char> black;

    black.resize(width*height*4);

    write(&black[0], width, height, 0,0,mipmaps,unit,informat,outformat);

}
