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

    Wrappy wrapy;

    EffectorRef ref("wrapy", &wrapy);

    logger.cout(5);

    // PLOGD << ref.wrapy->source();

    PLOGD << "pidooouu";

    exit(0);

    engine.run();

}
