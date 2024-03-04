#include "test.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "node.hpp"
#include "drawcall.hpp"
#include "component.hpp"
#include "effector.hpp"
#include "model.hpp"
#include "json.hpp"


#include <vector>
#include <map>
#include <string>

static int nextFactor2(int x, int factor = 4) { return ((int)(x/(float)factor)+1)*factor; }

Test::Test() {

    logger.cout();
    
    // auto comps = engine.tree->child("Debug::Components");
    // comps->childrens.resize(0);
    // for (auto c : Component::pool) comps->addPtr<Component>(c);


    // atlas is fucked


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
