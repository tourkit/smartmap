#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "builder.hpp"

struct BoilerQuad;

struct DrawCall : Modelable {

    DrawCall(std::string name = "DrawCall");

    ShaderProgram shader;

    virtual void draw();

    VBO vbo;

    Model* addModel(File* f) override;

    struct ShaderProgramBuilder : Builder {

        void build() override;

        DrawCall* dc;

        ShaderProgramBuilder(DrawCall* dc = nullptr);

        int stride_count = 0;

        std::string print_layer(Effectable &effectable,std::string prepend ,std::string instance, std::string ar = "");

    };

    ShaderProgramBuilder builder;

};
