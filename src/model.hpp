#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"

struct Model : Struct {

    File* file;

    Model();

    Model(File* file, int quantity = 1);

    ~Model();

    Effector& add(File* file);

    bool remove(Effector* effector);

    std::vector<Effector*> effectors;

};
