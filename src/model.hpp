#pragma once

struct File;
struct Effector;

#include <list>
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

    std::list<Effector> effectors;

};
