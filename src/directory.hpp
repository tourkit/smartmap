#pragma once

#include "file.hpp"


struct Directory {

    std::string path;

    std::vector<File*> list;
    
    Directory(std::string path);

    ~Directory();

    bool import(std::string path);

    static bool exist(std::string path) { return std::filesystem::is_directory(REPO_DIR+path); }

    void each(std::function<void(File*)> cb);

    const File& operator[](int x);

    auto begin() { return list.begin(); }
    auto end() { return list.end(); }

private:

    DIR *dir;

};
