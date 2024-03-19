/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"

#include "vbo.hpp"

int main() {

    logger.cout();

    engine.init();


    auto &vbo = *engine.tree->addOwnr<VBO>()->select()->get();


    vbo.vertices.push();
    vbo.print();
    // PLOGD << vbo.vertices.size();

    

    // engine.open("project2.json");

    engine.gui->editors.push_back(new EditorWidget());
    engine.run();
    
    
}