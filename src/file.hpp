#pragma once

#include <dirent.h>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <functional>


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
