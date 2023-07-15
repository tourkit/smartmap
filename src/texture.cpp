#include "texture.hpp" 

Texture::Texture() { pool.push_back(this); }

Texture::Texture(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y, GLuint unit, GLenum format) 
: Texture() { create(data, width, height, offset_x, offset_y, unit, format); }

static uint8_t FULLBLACK[4] = {0,0,0,0};
Texture::Texture(std::string path) : Texture()  {

    this->path = path; 
    Image img(path);
    if (!img.width) create(&FULLBLACK,1,1,0,0,0,GL_RGB8);
    else create(&img.data[0], img.width, img.height,0,0,0,GL_RGB8);

} 

Texture::~Texture() { destroy(); }

void Texture::destroy() { if (id) glDeleteTextures(1, &id);  }

void Texture::bind(int unit) { this->unit = unit; bind(); }

void Texture::bind() { glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, id); glActiveTexture(GL_TEXTURE0); }

Texture::operator GLuint() { return id; }

void Texture::create(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y, GLuint unit, GLenum format) {

    this->unit = unit; 
    this->width = width;
    this->height = height;
    this->format = format;

    destroy();

    glGenTextures(1, &id);

    glActiveTexture(GL_TEXTURE0+unit); 
    
    glBindTexture(GL_TEXTURE_2D, id); 

    glTexStorage2D(GL_TEXTURE_2D, mipmaps, format, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glActiveTexture(GL_TEXTURE0); 

    if (data) write(data, width, height, offset_x, offset_y);


}


void Texture::write(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y) {

    glActiveTexture(GL_TEXTURE0+unit); 
    
    glBindTexture(GL_TEXTURE_2D, id);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,offset_x,offset_y,width,height,GL_RGB,GL_UNSIGNED_BYTE,data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);

}

void Texture::read(const Texture* texture, GLuint offset_x, GLuint offset_y) {
    
    glCopyImageSubData(texture->id, GL_TEXTURE_2D, 0, 0,0, 0, id, GL_TEXTURE_2D, 0, offset_x, offset_y, 0, texture->width,texture->height, 1);

    GL_ERROR();

}