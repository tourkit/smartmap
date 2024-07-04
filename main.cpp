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

    builder.ubos.push_back(&engine.dynamic_ubo);
    builder.ubos.push_back(&engine.static_ubo);

    Wrappy wrapy;

    Effectable model;

    model.addEffector(&wrapy);

    logger.cout(5);

    shader.builder(&builder);

    shader.builder()->build();

    for (auto ref : model.refs) ref.get()->effector->setup(&shader);

    PLOGD << shader.builder()->fragment;

    PLOGD << shader.builder()->vertex;

    Struct quad("quad");
    quad.add<float>("x").add<float>("y").add<float>("z");


    Struct z1("zzz");
    z1.ref(&quad);

    Struct z2("zzz");
    z2.ref(&quad);

    engine.dynamic_ubo.add(&z1);
    engine.dynamic_ubo.add(&z2);

    {

        engine.dynamic_ubo.each([&](Instance& inst) {

            auto m = inst.def();

            if (m->type() == typeid(Struct)) shader.builder()->add(m);

        });

        shader.builder()->add(&engine.dynamic_ubo);

    }

    PLOGD << "\n" << shader.builder()->print_structs();

    PLOGD << "pidooouu";

    exit(0);

    engine.run();

}
