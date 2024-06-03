#pragma once

#include "vbo.hpp"
#include "shader.hpp"

#include <vector>
#include <cstring>

struct Effectable {

    Struct s;

    Effectable(std::string name = "sdfklg" ) : s(name) { }

    std::vector<std::shared_ptr<Effector>> effectors;
    Effector* addEffector(File* f); // kinda ctor for effectors
    bool removeEffector(Effector* effector);

};

struct Modelable : Effectable {

    VBO vbo;

    std::vector<std::shared_ptr<Model>> models;
    Model* addModel(File* f); // kinda ctor for Model
    bool removeModel(Model* model);

};

struct DrawCall : Modelable {

    DrawCall(std::string name);

    ShaderProgram shader;

    Struct s;

    virtual void update();

    virtual void draw();

    struct ShaderProgramBuilder : ShaderProgram::Builder {

        std::vector<File*> effectors;

        void build() override;
        void frag() override;
        void vert() override;

        DrawCall* dc;

        ShaderProgramBuilder();

        ShaderProgramBuilder(DrawCall* dc);

        int stride_count = 0;

    };

    // Builder* builder() { }

    ShaderProgramBuilder builder;

};
