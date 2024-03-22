/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"

#include "ubo.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "model.hpp"

void ttt(Member* www) {}


int main() {


    logger.cout(true);

    engine.init();

    auto &vbo_n = *engine.tree->addOwnr<VBO>()->select();
    vbo_n.onrun([](Node* node) { node->is_a<VBO>()->draw(); });
    vbo_n.active(true);
    
    ;
    vbo_n.addOwnr<File>("assets/models/quad.obj");
    // vbo_n.get()->import(new File("assets/models/quad.obj"));
    // auto &model = vbo_n.get()->models.back();
    // vbo_n.addPtr<Model>(&model);


    // model.import(new File("assets/effectors/argb.frag"));


    // engine.static_ubo.add<float>("test2");
    // vbo_n.get()->models[0].add<float>("test");
    // engine.dynamic_ubo["quad0"]["test"].set<float>(.2);

    auto shader = engine.tree->addOwnr<ShaderProgram>()->select()->get(); 
    
    engine.dynamic_ubo.bind(shader);
    // engine.static_ubo.bind(shader);

    shader->use();

    // effectors



    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
    
}
