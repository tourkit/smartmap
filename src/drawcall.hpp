#pragma once

#include "vbo.hpp"
#include "shader.hpp"

#include <vector>
#include <cstring>

struct DrawCall {

    DrawCall(std::string name);

    VBO vbo;

    ShaderProgram shader;

    Struct s;

    Struct fxs_s;

    virtual void update();

    virtual void draw();

    std::vector<std::shared_ptr<Model>> models;

    std::vector<std::shared_ptr<Effector>> effectors;

    bool remove(Model* model);

    Effector* addEffector(File* f);

    bool removeEffector(Effector* effector);

    Model* add(File* f);

};
