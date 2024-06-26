#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "engine.hpp"
#include "model.hpp"

#include <vector>
#include <cstring>



struct DrawCall : Modelable {

    DrawCall(std::string name);

    Engine::Shader shader;

    virtual void update();

    virtual void draw();

    VBO vbo;

    Model* addModel(File* f) override;

    struct ShaderProgramBuilder : ShaderProgram::Builder {

        std::vector<File*> effectors;

        void build() override;
        void frag() override;
        void vert() override;
        void common() override;

        std::string prout(std::string xtra, Model& model);

        DrawCall* dc;

        ShaderProgramBuilder();

        ShaderProgramBuilder(DrawCall* dc);

        int stride_count = 0;

    };

    // Builder* builder() { }

    ShaderProgramBuilder builder;

};
