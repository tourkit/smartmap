#pragma once

struct File;
struct Object;
struct ShaderFX;

#include <vector>

struct Model {

    File * file;

    int id;

    Object* obj;

    std::vector<ShaderFX*> fxs; 

    Model(File* file, int id = 0);

    void addFX(ShaderFX* fx);

};