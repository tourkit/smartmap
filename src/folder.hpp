#pragma once

#include <string>
#include <vector>
#include <functional>

struct File;

struct Folder {

    std::string path;

    std::vector<File> files;

    Folder(std::string path, bool load = true);

    ~Folder();

    bool import(std::string path, bool load = true);

    static bool exist(std::string path);

};
