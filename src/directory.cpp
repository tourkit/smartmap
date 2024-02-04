#include "directory.hpp"
#include "file.hpp" // REPO_DIR
#include <filesystem>
#include <dirent.h>

Directory::~Directory()  { }

Directory::Directory(std::string path) { import(path); }

bool Directory::exist(std::string path) { return std::filesystem::is_directory(REPO_DIR+path); }

bool Directory::import(std::string path)  { 

    if (!exist(path)) return false;

    DIR *dir;

    this->path = path;

    struct dirent* ent;

    if ((dir = opendir((REPO_DIR+path).c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string entryName(ent->d_name);
            if (entryName == ".." || entryName == ".") continue;
            
            list.push_back(path+ent->d_name);

        }

        closedir (dir);

    }

    return true;

}