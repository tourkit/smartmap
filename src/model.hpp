#pragma once

struct File;
struct Object;
struct Entry;
struct Effector;

#include <vector>
#include "component.hpp"

struct Model {

    File * file = nullptr;

    int id;

    Object* obj;
    
    Entry* entry;

    std::vector<Effector*> effectors; 

    int quantity;

    Model(File* file, int id = 0, int quantity = 1);

    Model(const char *data, int id = 0, int quantity = 1);

    bool import(File* file, int id = 0, int quantity = 1);

    bool import(const char *data, int id = 0, int quantity = 1);

    void addFX(Effector*  effector);
    
    void removeFX(Effector*  effector);
    
    void changeFX(Effector*  effector, Component *new_component);

};