#pragma once

#include "file.hpp"

struct Model : File {

    std::vector<std::array<float,2>> vertices;

    std::vector<std::array<float,2>> uvs;

    std::vector<std::array<uint32_t,3>> indices;
    
    Model(std::string path);
       
    void convert();

};





