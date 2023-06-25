#include "file.hpp"

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#endif

static std::vector<std::string> explode(std::string const & s, char delim) {

    std::vector<std::string> result;

    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); ) result.push_back(std::move(token));

    return result;

}

namespace TourKit {

Directory::Directory()  {}

Directory::~Directory()  { }

Directory::Directory(std::string path_str) { load(path_str); }

void Directory::load(std::string path_str){

    this->path_str = path_str;

    struct dirent* ent;
    

    if ((dir = opendir(path_str.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string entryName(ent->d_name);

            if (entryName != ".." && entryName != ".") list.push_back(ent->d_name);


        }

        closedir (dir);

    }

}

void Directory::print() {  std::cout << size() << " file(s) in \"" << path_str << "\" : \n" << '\n'; int i = 0; for (std::string file : list) std::cout << ++i << " - " << file << '\n'; }

int Directory::size(){ return list.size(); }

std::string Directory::path(int i) { return path_str+list[i]; }

const std::string& Directory::operator[](int x) { return list[x]; }

File::File() {  }

File::File(std::string source) { load(source); }

File::~File() { }


char* File::load(std::string source) {


    data.clear();

    filename = source;

    /* V2 ... still not reloading file



    FILE* f = fopen(filename, "r");

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    char* ptr = new char[size+1];
    memset(ptr,0,size+1);

    fseek(f, 0, SEEK_SET);
    fread(ptr, sizeof(char), size, f);

    fclose(f);*/

    /* V1*/

    std::ifstream file(source.c_str(), std::ifstream::in | std::ifstream::binary);
    file.seekg(0, std::ios::end);
    std::streampos length(file.tellg());

    if (length) {

        file.seekg(0, std::ios::beg);
        size = static_cast<std::size_t>(length);
        data.resize(size + 1);
        memset(data.data(),0,size+1);
        file.read(data.data(), size);

    }

    file.clear();

    return data.data();

}

void File::write(const char* filename, std::string filestr){ write(filename,&filestr[0],filestr.length()); }

void File::write(const char* filename, const char* i, int o){

    std::fstream bin (filename, std::ios :: out | std::ios :: binary);
    bin.write(i,o);
    bin.close();

}

void File::print() {

    std::cout << "[" << filename << "]\n" << std::endl;
    for (int i = 0; i < size; ++i) std::cout << data[i];
    std::cout << "\n-----------\n" << std::endl;

}

Image::Image() {  }

Image::Image(std::string source, bool flipped) {

    if (flipped) loadflipped(source);
    else load(source);
    
}

Image::~Image() {  }

void Image::store(int clear) {

    data.clear();

    data.insert(data.begin(),&i[0],&i[0]+(width*height*comp));

    if (clear) stbi_image_free(i);

    i = &data[0];

}

void Image::load(std::string source, IMAGE_ENCODING encoding, int quality) {

    File file(source);

    if (file.size != -1) load((unsigned char*)file.data.data(), file.size);

    if (encoding != RAW) convert(encoding, quality);


    auto filename = explode(explode(source, '/').back(), '.');
    name = std::move(filename.front());
    extension = std::move(filename.back());

}

void Image::loadflipped(std::string source) {

    stbi_set_flip_vertically_on_load(true);
    load(source);
    stbi_set_flip_vertically_on_load(false);

}


void Image::load(unsigned char *i, int width, int height , int comp) {

    this->i = i;
    this->width = width;
    this->height = height;
    this->comp = comp;
    this->o = width*height*comp;


    store();

}

void Image::load(unsigned char *i, int o) {


    this->i = stbi_load_from_memory(i, o, &width, &height, &comp, 0);

    store(1);

}

void Image::convert(IMAGE_ENCODING type, int quality) {

    int rv = 0;

    o = 0;

    data.clear();

    auto callback = [](void *context, void *data, int size) {

        auto _this = (Image *)context;
        unsigned char * _data = (unsigned char *)data;
        _this->data.insert(_this->data.end(), _data, _data + size );
        _this->o += size;

    };


    switch (type) {

        case JPEG: rv = stbi_write_jpg_to_func(callback, this, width, height, comp, i, quality); break;

        case PNG: rv = stbi_write_png_to_func(callback, this, width, height, comp, i,8); break;

        case BMP: rv = stbi_write_bmp_to_func(callback, this, width, height, comp, i); break;

        case RAW:  break;

    }

    i = &data[0];

    if (rv == 0) std::cout << "rv error\n";

}
void Image::rawconvert(int destcomp) {

  std::vector<unsigned char> newdata;

  int pixcount = width*height;

  if (comp == 3) {

    if (destcomp > 3) {

      newdata.resize(pixcount*4);

      memset(&newdata[0],255,newdata.size());

      if (destcomp == 5) { for (int i = 0; i< pixcount; ++i)  {  newdata[i*4] = data[i*3+2]; newdata[i*4+1] = data[i*3+1];  newdata[i*4+2] = data[i*3]; } }

      else if (destcomp == 4) { for (int i = 0; i< pixcount; ++i)  memcpy(&newdata[i*4], &data[i*3], 3);}

    }

  }

  data = newdata;

  comp = destcomp;

}

void Image::write(const char* destination, int quality) {

    if (extension == "jpg") convert(JPEG,quality);
    else if (extension == "png") convert(PNG);
    else std::cout << "chaipaskeussétonimaj takapté ? (TourKit::Image::write)" << '\n';

    std::cout << "exvoto" << '\n';
    File::write(destination, (const char*)i, o);

}

void Image::print() { std::cout << "Image: " << name << " ." << extension << " (" << width << ", " << height << ") raw:" << (data.size()/(float)8000000) << "Mb - comp:"<< comp <<  '\n'; }

}
