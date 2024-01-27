#pragma once

#include <string>
#include <vector>
#include <functional>

struct File;

struct Directory {

    std::string path;

    std::vector<File*> list;
    
    Directory(std::string path);

    ~Directory();

    bool import(std::string path);

    static bool exist(std::string path);

    void each(std::function<void(File*)> cb);

    const File& operator[](int x);

    // operator std::string() { return };

    auto begin() { return list.begin(); }
    auto end() { return list.end(); }


};
