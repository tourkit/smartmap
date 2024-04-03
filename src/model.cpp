#include "model.hpp"
#include "engine.hpp"

Model::Model(File* file, int quantity) : Struct(file->name(), quantity), file(file) { engine.dynamic_ubo.add(this); }

Model::~Model() { engine.dynamic_ubo.remove(*this);  }

bool Model::remove(Effector* effector) {

    for (auto it = effectors.begin(); it != effectors.end(); ++it) {

        if (&*it == effector) {

            Struct::remove(*effector->s);

            effectors.erase(it);

            break;

        }

    }

    return 1;

}

Effector& Model::add(File* file) {

    auto &x = *effectors.emplace(effectors.end(), file);

    Struct::add(x.s);

    return x;

}
