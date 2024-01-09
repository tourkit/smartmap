#pragma once

#include "file.hpp"
#include "node.hpp"

struct Image {

    enum Encoding { JPEG, PNG, BMP, RAW, BGRA };

    std::string name, extension;

    int width = 0, height, comp;  // comp: 1 = lumi, 2 = ?, 3 = rgb, 4 = rgba, 5 = bgra

    std::vector<unsigned char> data;

    Image(std::string source);
    // ~Image();

    void read(std::string source);

    void convert(Encoding type, int quality = 80);

    void rawconvert(int destcomp);

    void write(const char* destination, int quality = 100);

};
