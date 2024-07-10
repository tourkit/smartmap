/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "log.hpp"
#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"
#include "drawcall.hpp"
#include "layer.hpp"



int main() {

    logger.cout(Sev::error);
    engine.init();

    engine.open("/home/user/cpp/smartmap/project.json");

    // S

    // Layer dc(100,100);

    // Layer::ShaderProgramBuilder builder(&dc);

    // Wrappy wrapy;

    // dc.addEffector(&wrapy);

    // builder.effectors_fragment.push_back(&wrapy);

    // logger.cout(5);

    // dc.shader.builder(&builder);

    // Struct argb("argb");
    // argb.add<float>("a").add<float>("r").add<float>("g").add<float>("b"); //.add<float>("x").add<float>("y").add<float>("z");

    // builder.build();

    // PLOGD << "\n" << builder.vert();
    // PLOGD << "\n" << builder.frag();

    // exit(0);

    // engine.tree->addPtr<Layer>(&dc)->select();

    engine.run();

}
