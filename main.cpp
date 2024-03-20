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

    // vbo[0].member->quantity(vbo[0].member->quantity()+1);
    // auto inst1 = vbo[0].eq(vbo[0].member->quantity()-1);
    
    // vbo[0].member->quantity(vbo[0].member->quantity()+1);
    // auto inst2 = vbo[0].eq(vbo[0].member->quantity()-1);

    // // vbo[0].member->quantity(vbo[0].member->quantity()+1);
    // // auto inst3 = vbo[0].eq(vbo[0].member->quantity()-1);

    // PLOGD << vbo[0].offset << " + " << inst1.offset;
    // PLOGD << vbo[0].offset << " + " << inst2.offset;
    // PLOGD << vbo[0].size();
    // PLOGD << vbo[0].member->footprint_all();
    // PLOGD << vbo.members[0]->footprint_all();
    // // PLOGD << inst3.offset;
    // PLOGD << vbo[1].member->name();

    // PLOGD << vbo[1].offset;

    // PLOGD << vbo[0][0].offset;
    // PLOGD << vbo[0][1].offset;
    // PLOGD << vbo[0][2].offset;

    // auto v3 = vbo[0].push();
    // PLOGD << v3.offset;
    // auto v1 = vbo[0].push();
    // PLOGD << v1.offset;
    // auto v2 = vbo[0].push();
    // PLOGD << v2.offset;
    // auto v4 = vbo[0].push()[0];
    // PLOGD << v4.offset;

    // vbo[0].member->print();

    // v1["UV"].set<glm::vec2>({88,66});
    // v2["Position"].set<glm::vec2>({88,66});
    // PLOGD << v.member->name();


    vbo.import(new File("assets/models/quad.obj"));
    // vbo.print();
    // vbo.printData();
    // PLOGD << vbo[0].size();

    

    // engine.open("project2.json");

    engine.gui->editors.push_back(new EditorWidget());
    engine.run();
    
    
}
