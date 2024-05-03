#include "model.hpp"
#include "engine.hpp"

Model::Model(File* f, std::string name) : s(name), file(f) {  };

Model::~Model() { }

bool Model::remove(Effector* effector) {

    // effectors.erase(effector);
    PLOGW << "TODO";

    return 1;

}

Effector* Model::add(File* file) {

    auto effector = effectors.insert(std::make_shared<Effector>(file, s.next_name(file->name()))).first->get();

    PLOGD << effector->ref.name();
    s.add(&effector->ref);

    return effector;

}
