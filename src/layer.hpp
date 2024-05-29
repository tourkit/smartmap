#pragma once

#include "struct.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "engine.hpp"
#include "drawcall.hpp"

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



struct UberLayer : Layer {

    static inline Struct &layer_def = Struct::create("Layer", 0).add<glm::vec2>("size").add<glm::vec2>("pos");

    Instance* glsl_layers;

    struct VLayer {

        int w ;
        int h;
        int id = 0;
    };

    std::vector<VLayer> layers;

    int w ; int h;

    GLint max_tex_size;

    UberLayer() ;

    void calc_matrice(VBO* vbo) ;

    void addLayer(int w , int h) ;

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

};
