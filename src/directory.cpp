#include "directory.hpp"

Directory::~Directory()  { 
    
    // need to delete dir ? dont think , remove on comfirm 
}

Directory::Directory(std::string path) : path(path) { 

    struct dirent* ent;

    if ((dir = opendir(path.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string entryName(ent->d_name);

            if (entryName != ".." && entryName != ".") list.push_back(ent->d_name);

        }

        closedir (dir);

    }

}
