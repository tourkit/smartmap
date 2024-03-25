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



int main() { logger.cout(true); 

    // Boilerplate(); 
    
    engine.init(); engine.gui->editors.push_back(new EditorWidget());
    
    Boilerplate::Quad quad1,quad2;
    Boilerplate::Shader shader;

    auto &vbo_n = *engine.tree->addOwnr<VBO>();
    
    vbo_n.addPtr<Model>(&vbo_n.get()->add(new File("assets/models/quad.obj")));

    engine.tree->addOwnr<ShaderProgram>()->select()->get()->use();

    engine.tree->addOwnr<Node>("boil")->active(true)->onrun([&](Node* node){ 

        // quad2.draw();
        // quad2.draw();

        shader.use();

        vbo_n.get()->draw();

    });

    engine.run();
    
    
}
