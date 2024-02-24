#include "texture.hpp" 
#include "image.hpp" 
#include "log.hpp" 
#include "freetype.hpp" 


Texture::Texture(uint32_t width, uint32_t height, uint32_t unit, int mipmaps, GLenum informat, GLenum outformat) {

    create(width,height,unit,mipmaps,informat,outformat);

}

void Texture::create(uint32_t width, uint32_t height, uint32_t unit, int mipmaps, GLenum informat, GLenum outformat) {

    this->unit = unit;
    this->width = width;
    this->height = height;
    this->informat = informat;
    this->outformat = outformat;
    this->mipmaps = mipmaps;

    glGenTextures(1, &id); 

    PLOGV << width << " x " << height << " - id=" << id << " " << ", unit=" << unit << ", mipmaps=" << mipmaps;

    glActiveTexture(GL_TEXTURE0+unit); 

    glBindTexture(GL_TEXTURE_2D, id); 

    glTexStorage2D(GL_TEXTURE_2D, mipmaps, informat, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glActiveTexture(GL_TEXTURE0); 

}

Texture::Texture(void* data, uint32_t width, uint32_t height, uint32_t unit, int mipmaps, GLenum informat, GLenum outformat) { 

        create(width,height,unit,mipmaps,informat,outformat);
        
        write(data, width, height, 0, 0, unit, 1, informat, outformat); 
        
}

Texture::Texture(std::string path)   { 

    Image img(path);  

    if (!img.width) { PLOGW << "error in Texture creation" ; return; }
    
    create(img.width, img.height,0,1,GL_RGB8,GL_RGB);

    write(&img.data[0], img.width, img.height,0, 0,0,1,GL_RGB8,GL_RGB);

 }

void Texture::addChar(const char* chr,  int size, uint32_t offset_x, uint32_t offset_y) { 

    Freetype fr(chr, size);

    write(fr.buffer, fr.width, fr.height,offset_x, offset_y,0,1,GL_RGB8,GL_RED); 

}

void Texture::addImage(std::string path, uint32_t offset_x, uint32_t offset_y) { 

    Image img(path);  

    if (img.width) write(&img.data[0], img.width, img.height,offset_x, offset_y,0,1,GL_RGB8,GL_RGB);

} 

Texture::~Texture() { destroy(); }

void Texture::destroy() { 
    
    if (id) glDeleteTextures(1, &id); 

}

void Texture::bind(int unit) { this->unit = unit; bind(); }

void Texture::bind() { glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, id); glActiveTexture(GL_TEXTURE0); }

Texture::operator uint32_t() { return id; }
                    
void Texture::write(void* data, uint32_t width, uint32_t height, uint32_t offset_x, uint32_t offset_y, uint32_t unit, int mipmaps, GLenum informat, GLenum outformat) {

    glActiveTexture(GL_TEXTURE0+unit); 
    
    glBindTexture(GL_TEXTURE_2D, id);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D,0,offset_x,offset_y,width,height,outformat,GL_UNSIGNED_BYTE,data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);

}

void Texture::read(const Texture* texture, uint32_t offset_x, uint32_t offset_y) {
    
    glCopyImageSubData(texture->id, GL_TEXTURE_2D, 0, 0,0, 0, id, GL_TEXTURE_2D, 0, offset_x, offset_y, 0, texture->width,texture->height, 1);

    // GL_ERROR();

}

void Texture::clear() {

    std::vector<char> black;

    black.resize(width*height*4);

    write(&black[0], width, height, 0,0,mipmaps,unit,informat,outformat);

}