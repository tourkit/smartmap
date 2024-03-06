#include "model.hpp"

#include "effector.hpp"
#include "file.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "object.hpp"
#include "instance.hpp"
#include "ubo.hpp"

Model::Model(File* file, int id, int quantity) { import(file,id,quantity); }

bool Model::import(File* file, int id, int quantity) { 

    this->file = file;
    this->id = id;
    this->quantity = quantity;

    name = file->name+""+std::to_string(id);

    obj = engine.dynamic_ubo->addObj(new Struct(name.c_str()));

    entry = &obj->push();

    return true;

}

    
void Model::changeFX(Effector*  effector, Component *new_component){

    
    // obj->removeComp({effector->file->name});
    // Component::erase(effector->file->name.c_str()); // delete
    // Component::pool.push_back(new_component);
    // obj->addComp({new_component->name});

}


void Model::removeFX(Effector* effector) {

    auto x = std::remove(effectors.begin(), effectors.end(), effector);
    effectors.erase(x, effectors.end());

    obj->removeComp({effector->file->name});
    

}

void Model::addFX(Effector* effector) {

    obj->addComp({effector->file->name});

    // obj->resize(obj->reserved); // would fuck everything for moving data in heap // should add safety though
    
    effectors.push_back(effector);

}