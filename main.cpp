/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"



int main() {

    engine.init();

    ShaderProgram shader;

    ShaderProgram::Builder builder;

    auto *builder_ = &builder;

    builder.ubos.push_back(&engine.dynamic_ubo);
    builder.ubos.push_back(&engine.static_ubo);

    Wrappy wrapy;

    Effectable model;

    model.addEffector(&wrapy);

    logger.cout(5);

    shader.builder(&builder);

    auto builder_2 =shader.builder();

    shader.builder()->build();

    for (auto ref : model.refs) ref.get()->effector->setup(&shader);

    PLOGD << shader.builder()->fragment;

    PLOGD << shader.builder()->vertex;

    Struct quad("quad");
    quad.add<float>("x").add<float>("y").add<float>("z");


    Struct z1("zzz");
    Struct q1("qqq");
    q1.add<float>("x");
    z1.ref(&quad);
    // z1.add(&q1);

    Struct z2("zzz");
    Struct q2("qqq");
    q2.add<float>("x");
    z2.ref(&quad);
    // z2.add(&q1);

    engine.dynamic_ubo.add(&z1);
    engine.dynamic_ubo.add(&z2);

    PLOGD << "\n" << builder.print_structs();

    PLOGD << "pidooouu";

    exit(0);

    engine.run();

}
