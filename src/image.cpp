#include "image.hpp"
#include "file.hpp"
#include "log.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#endif

Image::Image(std::string path) { read(path); }

void Image::read(std::string path) {

    File::read(path, 1);

    if (data.size() != -1) {

        auto pixels = stbi_load_from_memory((const stbi_uc*)&data[0],data.size(), &width, &height, &comp, 0);

        if (!pixels) { PLOGE << stbi_failure_reason(); loaded = false; return; }

        auto count = width*height*comp;

        data.clear();
        data.resize(count);
        memcpy(&data[0],&pixels[0],count);

        stbi_image_free(pixels);

    }

}

void Image::convert(Encoding type, int quality) {

    // int rv = 0;

    // o = 0;

    // data.clear();

    // auto callback = [](void *context, void *data, int size) {

    //     auto _this = (Image *)context;
    //     unsigned char * _data = (unsigned char *)data;
    //     _this->data.insert(_this->data.end(), _data, _data + size );
    //     _this->o += size;

    // };


    // switch (type) {

    //     case JPEG: rv = stbi_write_jpg_to_func(callback, this, width, height, comp, i, quality); break;

    //     case PNG: rv = stbi_write_png_to_func(callback, this, width, height, comp, i,8); break;

    //     case BMP: rv = stbi_write_bmp_to_func(callback, this, width, height, comp, i); break;

    //     case RAW:  break;

    // }

    // i = &data[0];

    // if (rv == 0) std::cout << "rv error\n";

}

void Image::rawconvert(int destcomp) {

//   std::vector<unsigned char> newdata;

//   int pixcount = width*height;

//   if (comp == 3) {

//     if (destcomp > 3) {

//       newdata.resize(pixcount*4);

//       memset(&newdata[0],255,newdata.size());

//       if (destcomp == 5) { for (int i = 0; i< pixcount; ++i)  {  newdata[i*4] = data[i*3+2]; newdata[i*4+1] = data[i*3+1];  newdata[i*4+2] = data[i*3]; } }

//       else if (destcomp == 4) { for (int i = 0; i< pixcount; ++i)  memcpy(&newdata[i*4], &data[i*3], 3);}

//     }

//   }

//   data = newdata;

//   comp = destcomp;

}

void Image::write(const char* destination, int quality) {

    // if (extension == "jpg") convert(JPEG,quality);
    // else if (extension == "png") convert(PNG);
    // else std::cout << "chaipaskeussétonimaj takapté ? (TourKit::Image::write)" << '\n';

    // std::cout << "exvoto" << '\n';
    // File::write(destination, (const char*)&data[0], data.size());

}
