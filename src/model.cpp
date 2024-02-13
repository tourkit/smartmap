#include "model.hpp"

#include "shaderfx.hpp"
#include "file.hpp"
#include "component.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "object.hpp"

Model::Model(File* file, int id) : file(file), id(id) { 

    obj = engine.dynamic_ubo->buffer.addObj(new Struct("model"));

}

void Model::addFX(ShaderFX* fx) {

    // if (!Component::id(fx->file->name.c_str())) 
    
    auto &c = Component::create(fx->file->name.c_str());

    for (int i = 1; i < fx->args.size(); i++) c.member<float>(fx->args[i].c_str());

    obj->s->addComp({fx->file->name});
    
    fxs.push_back(fx);

}