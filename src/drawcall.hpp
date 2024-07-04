#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "engine.hpp"
#include "model.hpp"
// #include "effector.hpp"

#include <vector>
#include <cstring>



struct DrawCall : Modelable {

    DrawCall(std::string name = "DrawCall");

    Engine::Shader shader;

    virtual void update();

    virtual void draw();

    VBO vbo;

    Model* addModel(File* f) override;

    struct ShaderProgramBuilder : ShaderProgram::Builder {

        void build() override;

        std::string print_model(std::string xtra, Model& model);

        DrawCall* dc;

        ShaderProgramBuilder(DrawCall* dc = nullptr);

        int stride_count = 0;

    };

    // Builder* builder() { }

    ShaderProgramBuilder builder;

};
