#pragma once

#include <memory>

#include "effector.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"
#include "drawcall.hpp"

struct Layer : DrawCall {

    struct Feedback : Effector {

        Layer* layer;

        Texture texture;

        Feedback(Layer* layer);

        bool setup(Builder* builder) override;
        
        void post(Builder* builder) override;

        std::string source() override;

        bool body(Builder* builder, std::string prepend = "") override;

    };

    FrameBuffer fb;

    Feedback* feedback_v = nullptr;
    
    virtual Feedback* feedback();

    Layer(uint16_t width = 0, uint16_t height = 0, std::string name = "");

    void draw() override;

    static inline auto layer_def = Member("Layers").quantity(0).add<float, 2>("dim");

    static inline Instance* glsl_layers;

    void update() override;

};

struct UberLayer;

struct UberEffector : Effector {

    UberLayer *ubl_v;

    uint32_t range[2] = {0,0};

    UberEffector(UberLayer* ubl = nullptr);
    
    void ubl(UberLayer* ubl);

    std::string  source() override;

    bool setup(Builder* builder) override;
    
    bool body(Builder* builder, std::string prepend = "") override;

};

struct UberLayer : Layer {

    struct Feedback : Layer::Feedback {

        UberLayer* ubl;
        
        Feedback(UberLayer* ubl);

        std::string  source() override;
        bool setup(Builder* builder) override;

    };

    struct VirtualLayer : Effectable {

        VirtualLayer(int w, int h, int id = 0) : Effectable("Vlayer"+std::to_string(id)), w(w), h(h), id(id), effector(this) {  }

        int w ;
        int h;
        int id;

        UberLayer* ubl;

        struct Effector : ::Effector {

            Effector(VirtualLayer* vlayer);

            VirtualLayer* vlayer;

            bool setup(Builder* builder) override;
            
            bool body(Builder* builder, std::string prepend = "") override;

        };
        
        Effector effector;

    };

    static inline auto uberlayer_def = Member("UberLayers").add<float, 2>("size").add<float, 2>("pos").add<float, 2>("norm").add<float, 2>("dim");

    std::vector<std::shared_ptr<VirtualLayer>> layers;

    Member uberlayer_m;

    int w ; int h;

    UberLayer() ;

    void calc_matrice() ;

    VirtualLayer& addLayer(int w , int h) ; // kinda ctor for VLaye

    struct ShaderProgramBuilder : DrawCall::ShaderProgramBuilder {

        void build() override;

        UberLayer* ubl;

        ShaderProgramBuilder(UberLayer* ubl);

    } builder;

    UberEffector effector;
    
    Layer::Feedback* feedback() override;

};
