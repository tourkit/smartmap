/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"

#include "vbo.hpp"
#include "shader.hpp"

int main() {

    logger.cout();

    engine.init();

    auto &vbo_n = *engine.tree->addOwnr<VBO>()->select();
    vbo_n.active(true);

    vbo_n.onrun([](Node* node) { 
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        node->is_a<VBO>()->draw(); });
        

    auto &vbo = *vbo_n.get();

    vbo.import(new File("assets/models/quad.obj"));

    File frag("assets/basic.frag");
    File vert("assets/basic.vert");

    auto &shader_n = *engine.tree->addOwnr<ShaderProgram>(frag.data, vert.data)->select();

    shader_n.get()->use();



    
    // engine.open("project2.json");

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
    
}
