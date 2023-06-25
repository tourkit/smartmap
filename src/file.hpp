#ifndef FILE_H
#define FILE_H

#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

static std::string readFile(const char* path) {

  std::string data;

  std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
  file.seekg(0, std::ios::end);
  std::streampos length(file.tellg());

  if (length) {

      file.seekg(0, std::ios::beg);
      data.resize(length);
      file.read(&data[0], length);

  }

  file.clear();

  return data;

};

namespace TourKit {

struct Directory {

    std::string path_str;

    std::vector<std::string> list;

    DIR *dir;

    Directory();

    ~Directory();

    Directory(std::string path);

    void load(std::string path);

    void print();

    int size();

    std::string path(int i);

    const std::string& operator[](int x);
};

enum IMAGE_ENCODING { JPEG, PNG, BMP, RAW, BGRA };

class File {

public:

    std::string filename;
    int size;
    std::vector<char> data;

    File();
    File(std::string source);
    ~File();

    char* load(std::string source);
    static void write(const char* filename, std::string filestr);
    static void write(const char* filename, const char* i, int o);
    void print();
};

class Image {

public:

    unsigned char *i;
    int width = 0, height, comp, o;  // comp: 1 = lumi, 2 = ?, 3 = rgb, 4 = rgba, 5 = bgra
    std::vector<unsigned char> data;

    std::string name,extension;

    Image();
    Image(std::string source, bool flipped = false);
    ~Image();

    void store(int clear = 0);
    void load(std::string source, IMAGE_ENCODING encoding = RAW, int quality = 80);
    void loadflipped(std::string source);
    void load(unsigned char *i, int width, int height , int comp);
    void load(unsigned char *i, int o);
    void convert(IMAGE_ENCODING type, int quality = 80);

    void rawconvert(int destcomp);
    void write(const char* destination, int quality = 100);
    void print();
};

}

using Image = TourKit::Image;

#endif
