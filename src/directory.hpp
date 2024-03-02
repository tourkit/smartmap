#pragma once

#include <string>
#include <vector>
#include <functional>

struct Directory {

    std::string path;

    std::vector<std::string> list;
    
    Directory(std::string path);

    ~Directory();

    bool import(std::string path);

    static bool exist(std::string path);

    // operator std::string() { return };

    auto begin() { return list.begin(); }
    auto end() { return list.end(); }


};

struct File;

struct Folder : Directory {

    Folder(std::string path);

    ~Folder();
    
    std::vector<File*> files;
    
};