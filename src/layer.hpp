#pragma once

#include <memory>

#include "effector.hpp"
#include "struct.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"
#include "drawcall.hpp"

struct Layer : DrawCall {

    FrameBuffer fb;

    Texture* feedback = nullptr;

    Layer(uint16_t width = 0, uint16_t height = 0, std::string name = "");

    void draw() override;

    static inline Struct &layer_def = Struct::create("Layers",0).add<glm::vec2>("dim");

    static inline Instance* glsl_layers;

    void update() override;

};

struct UberLayer;

struct UberEffector : Effector {

    UberLayer *ubl_v;

    UberEffector(UberLayer* ubl = nullptr);
    
    void ubl(UberLayer* ubl);

    std::string  source() override;

    bool setup(Builder* builder) override;
    
    bool body(Builder* builder, std::string prepend = "") override;

};

struct UberLayer : Layer {

    struct VLayer : Effectable {

        VLayer(int w, int h, int id = 0) : Effectable("Vlayer"+std::to_string(id)), w(w), h(h), id(id) {  }

        int w ;
        int h;
        int id;

    };

    static inline Struct &uberlayer_def = Struct::create("UberLayers").add<glm::vec2>("size").add<glm::vec2>("pos").add<glm::vec2>("norm").add<glm::vec2>("dim");

    std::vector<std::shared_ptr<VLayer>> layers;

    Struct uberlayer_s;

    Instance *glsl_uberlayer;

    int w ; int h;

    UberLayer() ;

    void calc_matrice() ;

    VLayer& addLayer(int w , int h) ; // kinda ctor for VLaye

    struct ShaderProgramBuilder : DrawCall::ShaderProgramBuilder {

        void build() override;

        UberLayer* ubl;

        ShaderProgramBuilder(UberLayer* ubl);

    } builder;

    UberEffector effector;

};
