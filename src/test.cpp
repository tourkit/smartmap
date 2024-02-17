#include "test.hpp"
#include "log.hpp"

#include "engine.hpp"
#include "struct.hpp"
#include "object.hpp"
#include "entry.hpp"

Test::Test() {

    auto obj = engine.dynamic_ubo->addObj(new Struct("MICHEL"));

    obj->s->addComp({"float"});

    auto e1 = obj->push();

    obj->addComp({"Vec2"}); 

    e1[0][0].set<float>(69);
    auto e2 = obj->push();

    // auto z = *(std::vector<char>*)&e1[0][0].get<char>();

    // PLOGD << z.size();

}