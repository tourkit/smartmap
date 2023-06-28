#include "texture.hpp" 

Texture::Texture() { pool.push_back(this); }

Texture::Texture(void* data, GLuint width, GLuint height) : Texture() { create(data, width, height); }
Texture::Texture(std::string src) : Texture()   { create(src); } 

Texture::~Texture() { destroy(); }

void Texture::destroy() { if (id) glDeleteTextures(1, &id); }

void Texture::create(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y) {


    this->width = width;
    this->height = height;

    destroy();

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexStorage2D(GL_TEXTURE_2D, mipmaps, format, width, height);


    if (data) update(data, width, height, offset_x, offset_y);

}

void Texture::update(void* data, GLuint width, GLuint height, GLuint offset_x, GLuint offset_y) {

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, id);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,offset_x,offset_y,width,height,GL_RGB,GL_UNSIGNED_BYTE,data);

    glGenerateMipmap(GL_TEXTURE_2D);


}



static uint8_t FULLBLACK[4] = {0,0,0,0};
// static uint8_t FULLBLACK[4] = {255,255,255,255};

void Texture::reset() {create(path); }
void Texture::create(std::string src, GLuint offset_x, GLuint offset_y) {

    this->path = src; 

    Image img("C:/msys64/home/SysErr/old/smartmap/assets/media/"+std::string(src));
    if (!img.width) return create(&FULLBLACK,1,1);
    create(img.i, img.width, img.height, offset_x, offset_y);

}

void Texture::copy(const Texture& texture, GLuint offset_x, GLuint offset_y) {
    
    glCopyImageSubData(texture.id, GL_TEXTURE_2D, 0, 0,0, 0, id, GL_TEXTURE_2D, 0, offset_x, offset_y, 0, texture.width,texture.height, 1);

    GL_ERROR();

}

void Texture::bind(int unit) { glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, id); glActiveTexture(GL_TEXTURE0); }

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, id); }


Texture::operator GLuint() { return id; }