#include "model.hpp"

#include "shaderfx.hpp"
#include "file.hpp"
#include "component.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "object.hpp"
#include "entry.hpp"

Model::Model(File* file, int id, int quantity) : file(file), id(id), quantity(quantity) { 

    obj = engine.dynamic_ubo->addObj(new Struct((file->name+"_"+std::to_string(id)).c_str()));

    // obj->push();
    // entry[0][0].set<float>(1);

}

void Model::addFX(ShaderFX* fx) {
    
    auto &c = Component::create(fx->file->name.c_str());

    for (int i = 1; i < fx->args.size(); i++) c.member<float>(fx->args[i].c_str());

    obj->s->addComp({fx->file->name});

    // obj->resize(obj->reserved);
    
    fxs.push_back(fx);

}