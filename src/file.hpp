#pragma once

#include <string>
#include <cstdint>
#include <functional>

struct File {

    std::string name, path, location, extension;

    std::string data;

    int64_t last_modified = 0;

    bool survey = false, loaded = false;

    void update();
    
    std::function<void(File*)> callback = [](File* f){};

    File();

    File(std::string source);

    int64_t getTimeModified();

    void read(std::string source, bool binary = true);
    void write(const char* path);
    static void write(std::string path, std::string data);

    void reload();

    bool hasChanged();
    
};

#ifdef ROCH
static std::string REPO_DIR = "C:/users/root/cpp/smartmap/";
#else
static std::string REPO_DIR = "./";
#endif
