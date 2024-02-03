#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <string>

struct File;

struct Model {

    std::string name = "model";

    std::vector<std::array<float,2>> vertices;

    std::vector<std::array<float,2>> uvs;

    std::vector<std::array<uint32_t,3>> indices;
    
    Model(File *file);
       
    void import(File *file);

};