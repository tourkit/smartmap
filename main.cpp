/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "buffer.hpp"
#include "log.hpp"

int main() {
    
    logger.cout();

    Editors::init();

    for (auto i = 0 ; i < 24; i++)  PLOGD << nextFactor(i,16);

    // engine.init();

    Component::create("ID")
        .member<uint32_t>()
    ;
    Component::create("rectanlge")
        .member<glm::vec2>("pos").range(-1,1)
        .member<glm::vec2>("size")
        .member<float>("angle")
    ;

    Buffer buffy;
    auto obj = buffy.addObj(new Struct("Test",{"ID", "ID", "rectanlge"}));
    obj->is_striding = true;
    obj->s->stride(true);

    auto e1 = obj->push();
    
    engine.tree->addPtr<Buffer>(&buffy)->select();
    engine.gui->editors.push_back(new EditorWidget());

    engine.run();

}