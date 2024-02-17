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
    e1[0][0].set<float>(1);

    obj->s->addComp({"Vec2"});

    auto e2 = obj->push();
    // e1[1][1].set<float>(1);
    e2[1][0].set<float>(-1);

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    PLOGD << "notcrashed";

}