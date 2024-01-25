#pragma once

#include "pch.hpp"
#include "node.hpp"

struct File : Node {

    static inline std::vector<File*> pool;

    std::string path, location, filename, extension;

    std::string data;

    int64_t last_modified = 0;

    bool loaded = false;

    bool survey = false;

    void update();
    
    std::function<void(File*)> callback = [](File* f){};

    File();
    File(std::string source);

    int64_t getTimeModified();

    std::string read(std::string source, bool binary = true);
    void write(const char* path);
    static void write(std::string path, std::string data);
    
};
