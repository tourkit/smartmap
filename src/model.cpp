#include "model.hpp"
#include "engine.hpp"

Model::Model(File* f, std::string name) : s(name), file(f) {  };

Model::~Model() { }

bool Model::remove(Effector* effector) { return std::erase_if( effectors, [&](std::shared_ptr<Effector> e) { return e.get() == effector; }); }

Effector* Model::add(File* file) {

    auto effector = effectors.insert(effectors.end(), std::make_shared<Effector>(file, s.next_name(file->name())))->get();

    s.add(&effector->ref);

    return effector;

}
