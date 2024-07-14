#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <filesystem>


struct File {

    std::string name_v, path_v, location_v, extension;
    std::string filename();

    std::string data;

    int64_t last_modified = 1;

    bool owned = false;
    bool loaded = false;

    void update();

    std::string name();

    void name(std::string name);

    std::function<void(File*)> callback = [](File* f){};

    File();

    File(std::string source, bool load = true);

    File(std::string source, const char* value);

    int64_t getTimeModified();

    void loadString(std::string data);
    void read(std::string path, bool binary = false);
    void write(const char* data);
    static void write(std::string path, std::string data);

    std::string location();

    void location(std::string val);
    void path(std::string val);

    void reload();

    bool hasChanged();

    #ifdef ISUNIX
    static inline const char* REPO_DIR = "./";
    #else
    static inline const char* REPO_DIR = "C:/users/root/cpp/smartmap/";
    #endif

    static std::string loc() { return REPO_DIR; }

};
