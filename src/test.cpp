#include "test.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "node.hpp"

#include <vector>

Test::Test() {


    auto p = engine.stack->child("layer1::MyQuad");
    if (!p) PLOGW << "nop";


    // dc->is_a<Layer>()->shader.sendUniform("texture0", (int)tex->id);
    // tex->bind();
    // auto tex = new Texture("assets/medias/boy.jpg");
    // ndi->onrun([](Node* n) { 

    //     auto sender = n->is_a<NDI::Sender>();
    //     sender->send()

    //  });

    // auto dc = tree->addOwnr<DrawCall>();;
    // this->dc = dc->node();
    // dc->name = "engine";
    // dc->addPtr(models->childrens[0]);

}