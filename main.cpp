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

    // ShaderProgram::Builder builder;

    Wrappy wrapy;

    Effectable model;

    model.addEffector(&wrapy);

    logger.cout(5);

    shader.builder()->build();

    for (auto ref : model.refs) ref.get()->effector->setup(&shader);

    PLOGD << shader.builder()->fragment;

    PLOGD << shader.builder()->vertex;

    PLOGD << "pidooouu";

    exit(0);

    engine.run();

}
