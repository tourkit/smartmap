#include "model.hpp"

#include "effector.hpp"
#include "file.hpp"
#include "component.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "object.hpp"
#include "entry.hpp"

Model::Model(File* file, int id, int quantity) : file(file), id(id), quantity(quantity) { 

    obj = engine.dynamic_ubo->addObj(new Struct((file->name+""+std::to_string(id)).c_str()));

    obj->push();
    // entry[0][0].set<float>(1);

}

void Model::addFX(Effector* effector) {
    
    Component* c = Component::exist(effector->file->name.c_str());

    if (!c) {
        
        c = &Component::create(effector->file->name.c_str());

        for (auto arg : effector->args) {

            if (arg.first == "vec2") c->member<glm::vec2>(arg.second.c_str()); 
            
            else c->member<float>(arg.second.c_str()); 

            if (effector->ranges.find(arg.second) != effector->ranges.end()) c->range(effector->ranges[arg.second][0],effector->ranges[arg.second][1],effector->ranges[arg.second][2]);
            
        }
    
    }

    obj->addComp({effector->file->name});

    // obj->resize(obj->reserved); // would fuck everything for moving data in heap
    
    effectors.push_back(effector);

}