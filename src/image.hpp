#pragma once

#include <string>

#include "file.hpp"

struct Image : File {

    enum Encoding { JPEG, PNG, BMP, RAW, BGRA };

    std::string extension;

    int width = 0, height = 0, comp = 0;  // comp: 1 = lumi, 2 = ?, 3 = rgb, 4 = rgba, 5 = bgra

    Image(std::string source);
    // ~Image();

    void read(std::string source);

    void convert(Encoding type, int quality = 80);

    void rawconvert(int destcomp);

    void write(const char* destination, int quality = 100);

};
