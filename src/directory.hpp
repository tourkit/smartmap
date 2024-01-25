#pragma once

#include "file.hpp"


struct Directory {

    std::string path;

    std::vector<File*> list;

    std::vector<std::string> paths;
    
    Directory(std::string path, bool load = false);

    ~Directory();

    bool import(std::string path, bool load);

    static bool exist(std::string path) { return std::filesystem::is_directory(REPO_DIR+path); }

    void each(std::function<void(File*)> cb);

    const File& operator[](int x);

    // operator std::string() { return };

    auto begin() { return paths.begin(); }
    auto end() { return paths.end(); }

private:

    DIR *dir;

};
