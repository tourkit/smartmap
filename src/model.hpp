#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"

struct Model {

    File* file;

    Struct s;

    Model(File* f, std::string name);

    ~Model();

    Effector* add(File* file);

    bool remove(Effector* effector);

    std::set<std::shared_ptr<Effector>> effectors;

};
