#pragma once

#include "file.hpp"


struct Directory {

    std::string path;
    std::vector<std::string> list;
    DIR *dir;

    Directory(std::string path);
    ~Directory();

    static bool exist(std::string path) {
        
        return std::filesystem::is_directory(path);

    }

    const std::string& operator[](int x) { return list[x]; }
    operator std::vector<std::string>&() { return list; }

};
