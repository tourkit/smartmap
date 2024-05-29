#pragma once

#include "vbo.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"

#include <vector>
#include <cstring>

struct UBO;
struct FrameBuffer;

struct DrawCall {

    DrawCall(std::string name);

    VBO vbo;

    ShaderProgram shader;

    Struct s;

    Struct fxs_s;

    void update();

    virtual void draw();

    std::vector<std::shared_ptr<Model>> models;

    std::vector<std::shared_ptr<Effector>> effectors;

    bool remove(Model* model);

    Effector* addEffector(File* f);

    bool removeEffector(Effector* effector);

    Model* add(File* f);

};


struct Layer : DrawCall {

    struct ShaderProgramBuilder : ShaderProgram::Builder {

        std::set<File*> effectors; // must be filled by UBO which is not current solution // agreed twice

        void build() override;
        void frag() override;
        void vert() override;

        DrawCall* dc;

        ShaderProgramBuilder();

        ShaderProgramBuilder(DrawCall* dc);

        int stride_count = 0;

    };

    FrameBuffer fb;

    void draw() override;

    Layer(uint16_t width = 0, uint16_t height = 0, std::string name = "");

    Texture* feedback = nullptr;

};
