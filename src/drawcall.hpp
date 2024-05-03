#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"

#include <vector>
#include <cstring>

struct UBO;
struct FrameBuffer;

struct DrawCall {

    DrawCall();

    VBO vbo;

    ShaderProgram shader;

    Struct s;

    Struct fxs_s;

    void update();

    virtual void draw();

    std::set<std::shared_ptr<Model>> models;

    std::set<std::shared_ptr<Effector>> effectors;

    bool remove(Model* model);

    Model* add(File* f) {

        // if (f->extension == "glsl"){

        auto mod = models.insert(std::make_shared<Model>(f, s.next_name(f->name()))).first->get();

        s.add(&mod->s);

        return mod;

    }

};


struct Layer : DrawCall {

    static inline File *quad = new File("assets/models/quad.obj");

    FrameBuffer fb;

    void draw() override;

    Layer(uint16_t width, uint16_t height);

    Layer();

    Texture* feedback = nullptr;

};
