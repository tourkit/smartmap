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

File::File() : Node("file") {  }

File::File(std::string path) : Node(std::filesystem::path(path).stem().filename().string()) { read(path); }

int64_t File::getTimeModified() {

    auto last_modified_time = std::filesystem::last_write_time(path);

    std::int64_t last_modified_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(last_modified_time).time_since_epoch().count();

    return last_modified_ms;

}

void File::update() { read(path); }

void File::read(std::string path, bool binary){

    extension = std::filesystem::path(path).extension().string().substr(1);   
    location = std::filesystem::path(path).parent_path().string();
    this->path = path;


    auto flags = std::ifstream::in;
    if (binary) flags |= std::ifstream::binary;

    std::ifstream file(std::filesystem::path(REPO_DIR) / path, flags);

    if (file) {

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();

        file.seekg(0, std::ios::beg);
        data.resize(size);

        if (file.read(&data[0], size)) {
            file.close();
            last_modified = getTimeModified();

            PLOGV << path;

            loaded = true;
            return;
        }

    PLOGW <<1<<path;

    }
    
    else PLOGW <<2<< path;

    return;

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