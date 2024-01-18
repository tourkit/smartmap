#pragma once

#include "node.hpp"

struct Model : Node {

    std::vector<std::array<float,2>> vertices;

    std::vector<std::array<float,2>> uvs;

    std::vector<std::array<uint32_t,3>> indices;

    Model(std::string path = "quad.obj") {  Node::import(path); } 
    
    Model(File *file) {  import(file);}
       
    void import(File *file) override;

};





