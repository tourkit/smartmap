#pragma once

#include <memory>

#include "struct.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "engine.hpp"
#include "drawcall.hpp"

struct Layer : DrawCall {

    FrameBuffer fb;

    Texture* feedback = nullptr;

    Layer(uint16_t width = 0, uint16_t height = 0, std::string name = "");

    void draw() override;

    static inline Struct &layer_def = Struct::create("Layers",0).add<glm::vec2>("dim");

    static inline Instance* glsl_layers;

};

struct UberLayer : Layer {

    struct VLayer : Effectable {

        VLayer(int w, int h, int id = 0) : Effectable("Vlayer"+std::to_string(id)), w(w), h(h), id(id) {  }

        int w ;
        int h;
        int id;

    };

    static inline Struct &uberlayer_def = Struct::create("UberLayer",0).add<glm::vec2>("size").add<glm::vec2>("pos").add<glm::vec2>("pos_norm");

    std::vector<std::shared_ptr<VLayer>> layers;

    int w ; int h;

    UberLayer() ;

    void calc_matrice() ;

    VLayer& addLayer(int w , int h) ; // kinda ctor for VLaye

    struct ShaderProgramBuilder : DrawCall::ShaderProgramBuilder {

        void build() override;
        void frag() override;
        void vert() override;

        std::string print_layer(UberLayer::VLayer &layer);

        UberLayer* ubl;

        ShaderProgramBuilder(UberLayer* ubl);

    } builder;

};
