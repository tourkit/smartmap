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
    
    Component::create(fx->file->name.c_str())
        .member<float>("param1")
        .member<float>("param2")
        .member<float>("param3")
        ;

    obj->s->addComp({fx->file->name});
    
    fxs.push_back(fx);

}