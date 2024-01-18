#include "directory.hpp"

Directory::~Directory()  { 
    
    // need to delete dir ? dont think , remove on comfirm 
}

Directory::Directory(std::string path) { import(path); }

bool Directory::import(std::string path)  { 

    if (!exist(path)) return false;

    this->path = path;

    struct dirent* ent;

    if ((dir = opendir((REPO_DIR+path).c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string entryName(ent->d_name);

            if (entryName != ".." && entryName != ".") list.push_back(new File(path+ent->d_name));

        }

        closedir (dir);

    }

    return true;

}

const File& Directory::operator[](int x) { return *list[x]; }