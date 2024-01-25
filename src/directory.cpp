#include "directory.hpp"

Directory::~Directory()  { 
    
    for (auto f : list) delete f;
    PLOGD << "delete";
}

Directory::Directory(std::string path) { import(path); }

bool Directory::import(std::string path)  { 

    if (!exist(path)) return false;

    this->path = path;

    struct dirent* ent;

    if ((dir = opendir((REPO_DIR+path).c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string entryName(ent->d_name);
            if (entryName == ".." || entryName == ".") continue;
            
            auto t = new File(path+ent->d_name);

            if (!t->loaded) delete t; // needed ?
            if (t) {  // needed ?
                list.push_back(t);
                } //

        }

        closedir (dir);

    }

    return true;

}

const File& Directory::operator[](int x) { return *list[x]; }