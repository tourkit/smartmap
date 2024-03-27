#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"
#include "engine.hpp"

struct Model : Struct {

    File* file;

    Model(File* file, int quantity = 1) : Struct(file->name(), quantity) { engine.dynamic_ubo.add(*this); }

    Effector& add(File* file);

    bool remove(Effector* effector);

    std::vector<Effector> effectors;

};
