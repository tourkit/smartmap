#include "file.hpp"
#include "pch.hpp"



//////////////////////////////////////
// DIRECTOPRY
//////////////////////////////////////

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

//////////////////////////////////////
// FILE
//////////////////////////////////////



File::~File() {  

    auto x = std::remove(pool.begin(), pool.end(), this);
    pool.erase(x, pool.end());

}

File::File() { pool.push_back(this);  }

File::File(std::string path) : File() {  read(path); }

int64_t File::getTimeModified() {

    auto last_modified_time = std::filesystem::last_write_time(path);

    std::int64_t last_modified_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(last_modified_time).time_since_epoch().count();

    return last_modified_ms;

}

void File::update() { read(path); }

std::string File::read(std::string path, bool binary){

    path = (std::filesystem::path(REPO_DIR) / path).string();

    name = std::filesystem::path(path).filename().stem().string();  
    extension = std::filesystem::path(path).extension().string().substr(1);   


    this->path = path;

    auto flags = std::ifstream::in;
    if (binary) flags |= std::ifstream::binary;

    std::ifstream file(path, flags);

    if (file) {

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();

        file.seekg(0, std::ios::beg);
        data.resize(size);

        if (file.read(&data[0], size)) {
            file.close();
            last_modified = getTimeModified();
            return data;
        }

        

    }
    
    else std::cerr << "File Error : " << path << std::endl;

    return data;

}

void File::write(const char* filename){

    std::fstream bin (filename, std::ios :: out | std::ios :: binary);
    bin.write(&data[0],data.size());
    bin.close();

}

void File::write(std::string path, std::string data){

    std::fstream bin (path.c_str(), std::ios :: out | std::ios :: binary);
    bin.write(&data[0],data.size());
    bin.close();

}