/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "GL/gl3w.h"
#include "engine.hpp"

// #include "src/effector.hpp"

// maybe I need Member::Link to tell StrobeFX::static_s when StrobeFX::s has changed

// check how Member::update() works 

// could I use referings ?

// struct StrobeEffector : Effector {

//     int step = 2; 

//     Member* strobe_s;
    
//     ~StrobeEffector() { delete strobe_s; }

//     StrobeEffector() {

//         strobe_s = engine.glsl_data.add<int>("strobe").members.back();

//     }

//     bool setup(Builder* builder) override;
    
//     bool body(Builder* builder, std::string prepend = "") override;


// };



#include "boilerplate.hpp"
#include "drawcall.hpp"
#include "instance.hpp"
#include "layer.hpp"
#include "node.hpp"
#include "window.hpp"


int main() {

    //ubo not uploading

    // engine.init();
    logger.cout(Sev::warning);
    // engine.open("project.json");

    auto cc = engine.tree->addOwnr<Member>("coucou");
    auto cc_ = cc->is_a<Member>();
    cc_->add<float,3>("xyz");

    BoilerQuad quad;
    BoilerShader shader;
    Boilerplate::Editors();

    engine.tree->addPtr<BoilerShader>(&shader);

    glBindBuffer(GL_UNIFORM_BUFFER, engine.dynamic_ubo.id);
    glUniformBlockBinding(shader.id, glGetUniformBlockIndex(shader.id, "dynamic_ubo_"), 0); 
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, engine.dynamic_ubo.id);

    cc->on(Node::RUN,[&](Node* node) { 

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        shader.use();
        quad.draw();

    });

    engine.window.size(100,100);

    // auto &vbo_ = engine.stack->childrens[0]->is_a<Layer>()->vbo;


    engine.run();

}
