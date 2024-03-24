#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"
#include "engine.hpp"

struct Model : Struct {

    Struct* vbo;

    File* file;

    int quantity;

    Model(File* file, int quantity, Struct* vbo) : Struct(file->name(), quantity), vbo(vbo) { engine.dynamic_ubo.add(*this); }

    Effector& add(File* file);

    std::vector<Effector> effectors;

};