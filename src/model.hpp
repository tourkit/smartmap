#pragma once

struct File;
struct Object;
struct Effector;

#include <vector>

struct Model {

    File * file;

    int id;

    Object* obj;

    std::vector<Effector*> effectors; 

    int quantity;

    Model(File* file, int id = 0, int quantity = 1);

    void addFX(Effector*  effector);

};