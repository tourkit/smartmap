#pragma once

struct File;
struct Effector;

#include <vector>
#include <string>

#include "struct.hpp"
#include "effector.hpp"

struct Model : Struct {

    File* file;

    Model(File* file, int quantity = 1) : Struct (file->name(), quantity) { }

    int quantity;

};