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

    engine.static_ubo->add<float>("test2");

    Struct myeffector("MYEffecty");

    myeffector.add<float>("test");

    vbo_n.get()->models[0].add(myeffector);

    (*engine.dynamic_ubo)["quad0"]["MYEffecty"]["test"].set<float>(.2);

    auto shader = engine.tree->addOwnr<ShaderProgram>()->select()->get(); 
    
    engine.dynamic_ubo->bind(shader);
    engine.static_ubo->bind(shader);

    shader->use();

    // effectors


    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
    
}
