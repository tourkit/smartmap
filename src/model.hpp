#pragma once

struct File;
struct Object;
struct Entry;
struct Effector;

#include <vector>

struct Model {

    File * file;

    int id;

    Object* obj;
    
    Entry* entry;

    std::vector<Effector*> effectors; 

    int quantity;

    Model(File* file, int id = 0, int quantity = 1);

    void addFX(Effector*  effector);

};