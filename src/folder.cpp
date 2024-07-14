#include "folder.hpp"
#include "file.hpp"
#include <filesystem>
#include <dirent.h>

Folder::~Folder()  { for (auto x : files) delete x; }

Folder::Folder(std::string path, bool load) { import(path, load); }

bool Folder::exist(std::string path) { return std::filesystem::is_directory(File::loc()+path); }

bool Folder::import(std::string path, bool load)  {

    if (!exist(path)) return false;

    DIR *dir;

    this->path = path;

    struct dirent* ent;

    std::string dirpath = File::loc()+path;
    if ((dir = opendir(dirpath.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            std::string instanceName(ent->d_name);
            if (instanceName == ".." || instanceName == ".") continue;

            auto new_file = new File(path+ent->d_name, load);

            auto pos = 0;
            for (auto &file : files) {

                auto comp_pos = 0;
                while(true) if (new_file->filename()[comp_pos] == file->filename()[comp_pos]) comp_pos++; else break;
                if (new_file->filename()[comp_pos]< file->filename()[comp_pos]) break;

                pos++;
                
            }

            files.insert(files.begin()+pos, new_file);

        }

        closedir (dir);

    }

    return true;

}
