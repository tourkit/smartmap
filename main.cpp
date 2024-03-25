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
    
    engine.init(); engine.gui->editors.push_back(new EditorWidget());

    auto &vbo_n = *engine.tree->addOwnr<VBO>();
    vbo_n.active(true);
    
    auto &model = *vbo_n.addPtr<Model>(&vbo_n.get()->add(new File("assets/models/quad.obj")));

    model.addPtr<Effector>(&model.get()->add(new File("assets/effectors/argb.frag")))->get();

    auto shader = engine.tree->addOwnr<ShaderProgram>(vbo_n.get()->models)->select()->get();
    
    shader->use();

    engine.dynamic_ubo.bind(shader);

    engine.run();
    
}


//update shader