#pragma once

#include "instance.hpp"

struct Remap {

    Instance src, dst;

    virtual void update();

    int quantity;

    Remap(Instance src ,Instance dst, int quantity = 1);


};