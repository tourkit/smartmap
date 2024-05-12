#include "folder.hpp"
#include "file.hpp"
#include <filesystem>
#include <dirent.h>

Folder::~Folder()  { }

Folder::Folder(std::string path) { import(path); }

bool Folder::exist(std::string path) { return std::filesystem::is_directory(File::loc()+path); }

bool Folder::import(std::string path)  {

    if (!exist(path)) return false;

    DIR *dir;

    this->path = path;

    struct dirent* ent;

    std::string dirpath = File::loc()+path;
    if ((dir = opendir(dirpath.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string instanceName(ent->d_name);
            if (instanceName == ".." || instanceName == ".") continue;

            files.emplace_back(path+ent->d_name);

        }

        closedir (dir);

    }

    return true;

}
