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

    Struct myeffector("MYEffecty");

    myeffector.add<float>("test");

    vbo_n.get()->models[0].add(myeffector);

    (*engine.dynamic_ubo)["quad0"]["MYEffecty"]["test"].set<float>(.5);

    auto shader = engine.tree->addOwnr<ShaderProgram>()->select()->get(); 

    shader->use();

    glBindBuffer(GL_UNIFORM_BUFFER, engine.dynamic_ubo->id);
    glUniformBlockBinding(shader->id, glGetUniformBlockIndex(shader->id, engine.dynamic_ubo->name().c_str()), engine.dynamic_ubo->binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, engine.dynamic_ubo->binding, engine.dynamic_ubo->id);

    // effectors

    // // engine.open("project2.json");

    PLOGD<<"KA";
    // (*engine.dynamic_ubo)["quad0"]["MYEffecty"]["test"].set<float>(0.5);
    PLOGD<<"PIR";

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
    
}
