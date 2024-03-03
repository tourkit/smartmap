#include "file.hpp"
#include "log.hpp"
#include <filesystem>
#include <fstream>

static std::vector<std::string> explodefilename(std::string const & s, char delim) {

    std::vector<std::string> result;

    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); ) result.push_back(std::move(token));

    return result;

}

//////////////////////////////////////
// FILE
//////////////////////////////////////

File::File() { }

File::File(std::string path, bool binary) { read(path, binary); }

int64_t File::getTimeModified() {

    auto last_modified_time = std::filesystem::last_write_time(std::filesystem::path(REPO_DIR) / path);

    std::int64_t last_modified_ms = std::chrono::time_point_cast<std::chrono::nanoseconds>(last_modified_time).time_since_epoch().count();

    return last_modified_ms;

}

void File::update() { read(path); }

void File::reload() { read(path); }

bool File::hasChanged() { 

    if (loaded && last_modified != getTimeModified()) {

        last_modified = getTimeModified();

        return true;

    }

    return false;

}

void File::read(std::string path, bool binary){

    this->path = path;
    data.resize(0);

    loaded = false;
    
    auto flags = std::ifstream::in;
    if (binary) flags |= std::ifstream::binary;

    std::ifstream file(std::filesystem::path(REPO_DIR) / path, flags);

    if (file) {

        name = std::filesystem::path(path).stem().filename().string();

        extension = std::filesystem::path(path).extension().string().substr(1);   

        location = std::filesystem::path(path).parent_path().string();

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();

        if (!size) {PLOGW << "FAIL"; return;}

        file.seekg(0, std::ios::beg);

        data.resize(size);

        if (file.read(&data[0], size)) file.close();

        last_modified = getTimeModified();

        PLOGD << path << " " << size << "kb";
        
        loaded = true;

        return;
    

    }

    PLOGW << "couldn't find: " << path;

    return;

}
void File::loadString(std::string data) {

    this->data.resize(data.length());

    memcpy(&this->data[0],&data[0],data.length());
    
    loaded = true;

    last_modified = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

}

void File::write(const char* data){

    if (!loaded) return;

    File::write(path,data);

    reload();


}

void File::write(std::string path, std::string data){

    std::fstream bin (std::filesystem::path(REPO_DIR) / path.c_str(), std::ios :: out | std::ios :: binary);
    bin.write(&data[0],data.size());
    bin.close();

}