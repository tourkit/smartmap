#ifndef FILE_H
#define FILE_H

#include <dirent.h>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <functional>

// 
#define ROCH
#ifdef ROCH
static std::string REPO_DIR = "C:/users/root/cpp/smartmap/";
#else
static std::string REPO_DIR = "./";
#endif


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

    static inline std::vector<File*> pool;

    std::string path, name, extension;

    std::string data;

    int64_t last_modified = 0;

    bool survey = false;

    void update();
    
    std::function<void(File*)> callback = [](File* f){};

    File();
    File(std::string source);
    ~File();

    int64_t getTimeModified();

    std::string read(std::string source, bool binary = true);
    void write(const char* path);
    static void write(std::string path, std::string data);
    
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
