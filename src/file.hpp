#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <filesystem>


struct File {

    std::string name_v, path, location, extension;

    std::string data;

    int64_t last_modified = 1;

    bool loaded = false;

    void update();

    std::string name();

    std::function<void(File*)> callback = [](File* f){};

    File();

    File(std::string source, bool binary = false);

    int64_t getTimeModified();

    void loadString(std::string data);
    void read(std::string path, bool binary = false);
    void write(const char* data);
    static void write(std::string path, std::string data);

    void reload();

    bool hasChanged();

    #ifdef ROCH
    static inline std::string REPO_DIR = "C:/users/root/cpp/smartmap/";
    #else
    static inline std::string REPO_DIR = "./";
    #endif

};
