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


struct Directory {

    std::string path;
    std::vector<std::string> list;
    DIR *dir;

    Directory(std::string path);
    ~Directory();

    const std::string& operator[](int x) { return list[x]; }
    operator std::vector<std::string>&() { return list; }

};

struct File {

    std::string path;
    std::vector<char> data;

    File(std::string source);
    ~File();

    char* read(std::string source);
    void write(const char* path);

    const char& operator[](int x) { return data[x]; }
    operator std::vector<char>&() { return data; }

};



struct Image {

    enum Encoding { JPEG, PNG, BMP, RAW, BGRA };

    std::string name,extension;

    int width = 0, height, comp;  // comp: 1 = lumi, 2 = ?, 3 = rgb, 4 = rgba, 5 = bgra

    std::vector<unsigned char> data;

    Image(std::string source);
    // ~Image();

    void read(std::string source);

    void convert(Encoding type, int quality = 80);

    void rawconvert(int destcomp);

    void write(const char* destination, int quality = 100);

};

#endif
