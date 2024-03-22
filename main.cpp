/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"

#include "ubo.hpp"
#include "vbo.hpp"
#include "shader.hpp"

void ttt(Member* www) {}


int main() {


    logger.cout(true);

    engine.init();

    auto &vbo_n = *engine.tree->addOwnr<VBO>()->select();
    
    vbo_n.active(true);

    vbo_n.get()->import(new File("assets/models/quad.obj"));

    vbo_n.onrun([](Node* node) { node->is_a<VBO>()->draw(); });

    engine.static_ubo->Struct::add<float>("test2");

    Struct myeffector("MYEffecty");

    myeffector.add<float>("test");

    vbo_n.get()->models[0].add(myeffector);

    (*engine.dynamic_ubo)["quad0"]["MYEffecty"]["test"].set<float>(.5);

    // engine.dynamic_ubo->Buffer::update(); 
    
    glDeleteBuffers(1, &engine.dynamic_ubo->id);

    glGenBuffers(1, &engine.dynamic_ubo->id);

    glBindBuffer(GL_UNIFORM_BUFFER, engine.dynamic_ubo->id);

    glBufferData(GL_UNIFORM_BUFFER, engine.dynamic_ubo->data.size(), NULL, GL_DYNAMIC_COPY);

    engine.dynamic_ubo->upload();

    auto shader = engine.tree->addOwnr<ShaderProgram>()->select()->get(); // ubo striding here

    shader->use();

    // effectors


    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
    
}
