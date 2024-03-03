#include "test.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "node.hpp"
#include "drawcall.hpp"
#include "component.hpp"
#include "effector.hpp"
#include "model.hpp"
#include "json.hpp"

#include "../vendors/rapidjson/writer.h"

#include <vector>

static int nextFactor2(int x, int factor = 4) { return ((int)(x/(float)factor)+1)*factor; }

Test::Test() {

    logger.cout();

    // Create a JSON document

    if (!engine.json.document.HasMember("editors")) engine.json.document.AddMember("editors", rapidjson::Value(rapidjson::kArrayType), engine.json.document.GetAllocator());

    for (int i = 1; i < engine.gui->editors.size(); i++) {
    
        auto e = engine.gui->editors[i];

        auto v = rapidjson::Value(rapidjson::kArrayType);

        std::string name = "none";
        if (e->selected) name = e->selected->namesdf();

        v.PushBack(0, engine.json.document.GetAllocator());
        v.PushBack(0, engine.json.document.GetAllocator());
        v.PushBack(0, engine.json.document.GetAllocator());
        v.PushBack(0, engine.json.document.GetAllocator());
        v.PushBack(rapidjson::Value(name.c_str(), engine.json.document.GetAllocator()), engine.json.document.GetAllocator());

        auto &x = engine.json.document["editors"].PushBack(v, engine.json.document.GetAllocator());
        
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    engine.json.document.Accept(writer);
    PLOGW << buffer.GetString();


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
