#include "model.hpp"

Effector& Model::add(File* file) {

    auto &x = effectors.emplace_back(file);

    Struct::add(x);

    // vbo->update();

    return x;
    
}