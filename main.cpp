/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */

#include "GL/gl3w.h"
#include "callbacks.hpp"
#include "editors.hpp"
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
#include "file.hpp"
#include "instance.hpp"
#include "layer.hpp"
#include "node.hpp"
#include "window.hpp"


int main() {

    //ubo not uploading

    // engine.init();

    Callbacks::init();
    Editors::init();

    logger.cout(Sev::warning);
    // engine.open("project.json");

    auto cc = engine.tree->addOwnr<Member>("coucou");
    auto cc_ = cc->is_a<Member>();
    cc_->add<float,3>("xyz");

    BoilerQuad quad;
    BoilerShader shader;

    // BoilerUBO ubo2(shader.id);
    UBO ubo2("ubo");
    engine.tree->addPtr<UBO>(&ubo2)->active(false);
    Member test("test");
    test.add<int,4>("xyzw").range(0, 100, 10);
    ubo2.add(&test);
    Instance(ubo2).set<int,4>(11,22,10,100);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo2.id);
    glUniformBlockBinding(shader.id, glGetUniformBlockIndex(shader.id, ubo2.name().c_str()), ubo2.binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, ubo2.binding, ubo2.id);

    Boilerplate::Editors();

    engine.tree->addPtr<BoilerShader>(&shader)->on(Node::CHANGE, [&](Node* node) { 

        glBindBuffer(GL_UNIFORM_BUFFER, ubo2.id);
        glUniformBlockBinding(shader.id, glGetUniformBlockIndex(shader.id, ubo2.name().c_str()), ubo2.binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, ubo2.binding, ubo2.id);

     });

    // glBindBuffer(GL_UNIFORM_BUFFER, engine.dynamic_ubo->id);
    // glUniformBlockBinding(shader.id, glGetUniformBlockIndex(shader.id, "dynamic_ubo_"), 0); 
    // glBindBufferBase(GL_UNIFORM_BUFFER, 0, engine.dynamic_ubo->id);

    cc->on(Node::RUN,[&](Node* node) { 

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        shader.use();
        quad.draw();
        ubo2.upload();

    });

    engine.window.size(100,100);
    engine.window.pos(0,0);

    // auto &vbo_ = engine.stack->childrens[0]->is_a<Layer>()->vbo;


    engine.run();

}
