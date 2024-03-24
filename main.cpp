/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "boilerplate.hpp"

#include "ubo.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "model.hpp"



int main() {


    logger.cout(true); engine.init(); engine.gui->editors.push_back(new EditorWidget());
    
    Boilerplate::Quad quad;

    auto &vbo_n = *engine.tree->addOwnr<VBO>()->select();
    // vbo_n.addPtr<Model>(&vbo_n.get()->add(new File("assets/models/quad.obj")));
    // vbo_n.active(true);

    engine.tree->addOwnr<ShaderProgram>()->select()->get()->use();

    engine.tree->addOwnr<Node>()->active(true)->onrun([&](Node* node){ 
        
        quad.draw(); 
        // vbo_n.get()->draw();
        
    });

    engine.run();
    
    
}
