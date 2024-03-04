#include "directory.hpp"
#include "file.hpp" 
#include <filesystem>
#include <dirent.h>

Directory::~Directory()  { }

Directory::Directory(std::string path) { import(path); }

bool Directory::exist(std::string path) { return std::filesystem::is_directory(File::REPO_DIR+path); }

bool Directory::import(std::string path)  { 

    if (!exist(path)) return false;

    DIR *dir;

    this->path = path;

    struct dirent* ent;

    std::string dirpath = File::REPO_DIR+path;
    if ((dir = opendir(dirpath.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string entryName(ent->d_name);
            if (entryName == ".." || entryName == ".") continue;
            
            list.push_back(path+ent->d_name);

        }

        closedir (dir);

    }

    return true;

}

Folder::Folder(std::string path) : Directory(path) { for (auto f : list) files.push_back(new File(f)); }

Folder::~Folder() { for (auto f : files) delete f; }
