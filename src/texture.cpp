#include "texture.hpp" 

Texture::Texture(GLuint width, GLuint height, GLenum format, void* data) : width(width), height(height), format(format) {

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexStorage2D(GL_TEXTURE_2D, mipmaps, format, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    if (data) upload(data);

}

void Texture::upload(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y) {

    if (!width) width = this->width;
    if (!height) height = this->height;

    glBindTexture(GL_TEXTURE_2D, id);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,offset_x,offset_y,std::min(width,this->width),std::min(height,this->height),GL_RGB,GL_UNSIGNED_BYTE,data);

    glGenerateMipmap(GL_TEXTURE_2D); //if(mipmaps>1)?

}

void Texture::upload(std::string src, GLuint offset_x, GLuint offset_y) {

    Image img;
    img.loadflipped(src);
    upload(img.i, img.width, img.height, offset_x, offset_y);

}

void Texture::copy(const Texture& texture, GLuint offset_x, GLuint offset_y) {
    
    glCopyImageSubData(texture.id, GL_TEXTURE_2D, 0, 0,0, 0, id, GL_TEXTURE_2D, 0, offset_x, offset_y, 0, texture.width,texture.height, 1);

}

void Texture::bind(int unit) { glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, id); glActiveTexture(GL_TEXTURE0); }

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, id); }
