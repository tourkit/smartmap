/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"

#include "vbo.hpp"
#include "shader.hpp"

int main() {

    logger.cout(true);

    engine.init();

    engine.tree->addOwnr<ShaderProgram>()->select()->get()->use();
    
    auto &vbo_n = *engine.tree->addOwnr<VBO>()->select();
    
    vbo_n.active(true);

    vbo_n.get()->import(new File("assets/models/quad.obj"));

    vbo_n.onrun([](Node* node) { node->is_a<VBO>()->draw(); });

    // // engine.open("project2.json");

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
    
}
