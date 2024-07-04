/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"
#include "drawcall.hpp"
#include "layer.hpp"



int main() {

    engine.init();

    Layer dc;

    DrawCall::ShaderProgramBuilder builder(&dc);

    Wrappy wrapy;

    Effectable model;

    model.addEffector(&wrapy);

    builder.effectors_fragment.push_back(&wrapy);

    logger.cout(5);


    dc.shader.builder(&builder);

    auto builder_2 =dc.shader.builder();

    dc.shader.builder()->build();

    for (auto ref : model.refs) ref.get()->effector->setup(&dc.shader); // does it do shit ?

    PLOGD << dc.shader.builder()->frag();

    PLOGD << dc.shader.builder()->vert();

    Struct quad("quad");
    quad.add<float>("x").add<float>("y").add<float>("z");


    Struct z1("zzz");
    // Struct q1("qqq");
    // q1.add<float>("x");
    z1.ref(&quad);
    // z1.add(&q1);

    Struct z2("zzz");
    Struct q2("qqq");
    // q2.add<float>("x");
    z2.ref(&quad);
    // z2.add(&q1);

    engine.dynamic_ubo.add(&z1);
    engine.dynamic_ubo.add(&z2);

    builder.build();
    PLOGD << "\n" << builder.frag();

    PLOGD << "pidooouu";

    // exit(0);

    engine.run();

}
