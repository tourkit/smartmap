#include "model.hpp"

#include "effector.hpp"
#include "file.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "instance.hpp"
#include "ubo.hpp"

Model::Model(File* file, int id, int quantity) { import(file,id,quantity); }

bool Model::import(File* file, int id, int quantity) { 

    // tofix

    this->file = file;
    this->id = id;
    this->quantity = quantity;

    name = file->name+""+std::to_string(id);

    s = &Struct::create(name.c_str());

    engine.dynamic_ubo->add(*s);

    return true;

}

    
// void Model::changeFX(Effector*  effector, Component *new_component){

    
//     // obj->removeComp({effector->file->name});
//     // Component::erase(effector->file->name.c_str()); // delete
//     // Component::pool.push_back(new_component);
//     // obj->addComp({new_component->name});

// }


void Model::removeFX(Effector* effector) {

// tofix

    // auto x = std::remove(effectors.begin(), effectors.end(), effector);
    // effectors.erase(x, effectors.end());

    // obj->removeComp({effector->file->name});
    

}

void Model::addFX(Effector* effector) {

// tofix

    // obj->addComp({effector->file->name});

    // // obj->resize(obj->reserved); // would fuck everything for moving data in heap // should add safety though
    
    // effectors.push_back(effector);

}