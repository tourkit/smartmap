#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "builder.hpp"

struct BoilerQuad;

struct DrawCall : Modelable {

    DrawCall(std::string name = "DrawCall");

    virtual void draw();

    VBO vbo;

    Model* addModel(File* f) override;

    struct Builder : ::Builder {

        void setup() override;

        void use(DrawCall* dc = nullptr, int stride_count = 0);

        DrawCall* dc;

        int stride_count;

        std::string print_layer(Effectable &effectable,std::string prepend ,std::string instance, std::string ar = "");

    };

    static inline Builder builder;

    Builder& builder_v = builder;

    ShaderProgram shader;

    int GL_BLEND_MODE_IN = 6;
    int GL_BLEND_MODE_OUT = 1;//13

};
