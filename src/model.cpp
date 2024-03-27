#include "model.hpp"

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

    Struct::add(*x.s);

    return x;

}
