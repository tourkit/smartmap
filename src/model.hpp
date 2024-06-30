#pragma once

#include <vector>
#include <string>

#include "effector.hpp"

struct Model;

struct Modelable : Effectable {

    using Effectable::Effectable;

    std::vector<std::shared_ptr<Model>> models;
    virtual Model* addModel(File* f); // kinda ctor for Model
    bool removeModel(Model* model);

};

struct Model : Modelable {

    File* file;

    Instance* instance;

    Model(File* f, std::string name);

    ~Model();

    static void convert(File* model, std::string type);

};
