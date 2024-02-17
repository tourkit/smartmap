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
    
    auto &c = Component::create(effector->file->name.c_str());

    for (int i = 1; i <effector->args.size(); i++) c.member<float>(effector->args[i].c_str());

    obj->addComp({effector->file->name});

    // obj->resize(obj->reserved); // would fuck everything for moving data in heap
    
    effectors.push_back(effector);

}