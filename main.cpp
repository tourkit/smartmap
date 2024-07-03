/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"



int main() {

    engine.init();

    ShaderProgram::Builder builder;

    Effector def;

    EffectorRef effector("effector", &def);

    logger.cout(5);

    PLOGD << effector.source();

    exit(0);

    engine.run();

}
