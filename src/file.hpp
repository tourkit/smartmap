#ifndef FILE_H
#define FILE_H

#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

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
    std::string data;

    File();
    File(std::string source);
    ~File();

    std::string read(std::string source, bool binary = true);
    void write(const char* path);
    
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
