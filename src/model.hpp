#pragma once

struct File;
struct Struct;
struct Instance;
struct Effector;

#include <vector>
#include <string>

struct Model {

    File * file = nullptr;

    int id;

    Struct* s;
    
    // Instance* instance;

    std::vector<Effector*> effectors; 

    int quantity;

    std::string name;

    Model(File* file, int id = 0, int quantity = 1);

    Model(const char *data, int id = 0, int quantity = 1);

    bool import(File* file, int id = 0, int quantity = 1);

    bool import(const char *data, int id = 0, int quantity = 1);

    void addFX(Effector*  effector);
    
    void removeFX(Effector*  effector);
    
    // // void changeFX(Effector*  effector, Component *new_component);

};