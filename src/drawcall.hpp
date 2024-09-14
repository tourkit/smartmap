#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "builder.hpp"

struct BoilerQuad;

struct DrawCall : Modelable {

    
    static inline BoilerQuad* bquad = nullptr;

    DrawCall(std::string name = "DrawCall");

    ShaderProgram shader;

    virtual void draw();

    VBO vbo;

    Model* addModel(File* f) override;

    struct ShaderProgramBuilder : Builder {

        void build() override;

        DrawCall* dc;

        ShaderProgramBuilder(DrawCall* dc = nullptr);

        // std::string print_layer(UberLayer::VLayer &layer);

        int stride_count = 0;

        std::string print_layer(Effectable &effectable,std::string prepend ,std::string instance, std::string ar = "");

    };

    // Builder* builder() { }

    ShaderProgramBuilder builder;

};
