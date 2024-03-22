#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"

struct Model : Struct {

    // Data data ?

    Model(std::string name = "Model", int id = 0, int quantity = 1);

    int id;

    int quantity;

    std::vector<Effector*> effectors; 
 

};