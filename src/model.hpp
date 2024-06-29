#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"


struct Effector;
struct Model;
struct Effectable {

    Struct s;

    Effectable(std::string name = "Effectable" );

    std::vector<std::shared_ptr<Effector>> effectors;
    Effector* addEffector(File* f); // kinda ctor for effectors
    bool removeEffector(Effector* effector);

};

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
