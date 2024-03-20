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

    auto v1 = vbo.vertices.push()[0];


    v1["Position"].set<glm::vec2>({123,245});
    v1["UV"].set<glm::vec2>({88,66});
    // v2["Position"].set<glm::vec2>({88,66});
    // PLOGD << v.member->name();


    // vbo.import(new File("assets/models/quad.obj"));
    // vbo.print();
    // vbo.printData();
    // PLOGD << vbo.vertices.size();

    

    // engine.open("project2.json");

    engine.gui->editors.push_back(new EditorWidget());
    engine.run();
    
    
}
