#include "model.hpp"

#include "effector.hpp"
#include "file.hpp"
#include "component.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "object.hpp"
#include "entry.hpp"
#include "ubo.hpp"

Model::Model(File* file, int id, int quantity) { import(file,id,quantity); }

bool Model::import(File* file, int id, int quantity) { 

    this->file = file;
    this->id = id;
    this->quantity = quantity;

    obj = engine.dynamic_ubo->addObj(new Struct((file->name+""+std::to_string(id)).c_str()));

    entry = &obj->push();

    return true;

}

void Model::addFX(Effector* effector) {

    /// comp creation should move in effector.cpp // ach q iso tem q ir em effectchor.hppou outra veiiissss
    
    Component* c = Component::exist(effector->file->name.c_str());

    if (!c) {
        
        c = &Component::create(effector->file->name.c_str());

        for (auto arg : effector->args) {

            if (arg.first == "vec2") c->member<glm::vec2>(arg.second.c_str()); 

            else if (arg.first == "int") c->member<int>(arg.second.c_str()); 
            
            else c->member<float>(arg.second.c_str()); 

            if (effector->ranges.find(arg.second) != effector->ranges.end()) c->range(effector->ranges[arg.second][0],effector->ranges[arg.second][1],effector->ranges[arg.second][2]);
            
        }
    
    }

    obj->addComp({effector->file->name});

    // obj->resize(obj->reserved); // would fuck everything for moving data in heap // should add safety though
    
    effectors.push_back(effector);

}