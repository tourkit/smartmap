#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"
#include "drawcall.hpp"

struct Model : Modelable {

    File* file;

    Instance* instance;

    Model(File* f, std::string name);

    ~Model();

    static void convert(File* model, std::string type);

};
