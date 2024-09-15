

/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype

// grid / quantity 

// ----


#include "drawcall.hpp"
#include "engine.hpp"
#include "builder.hpp"
#include "instance.hpp"




// fix dynubo && statubo ..

// count Shader::create per NODE<Effectable>::add<Effector>

int main() {

    logger.cout(Sev::warning);


    engine.init();

    engine.open("project.json");

    auto dc = engine.tree->find("Stack")->childrens[0]->is_a<DrawCall>();
    auto quad1 = engine.tree->find("quad1");
    auto argb = engine.tree->find("argb");
    
    quad1->add(argb);

    engine.dynamic_ubo->bind(dc->shader);

    logger.cout(Sev::verbose);
    Instance(*engine.dynamic_ubo).print();

    logger.cout(Sev::warning);
    engine.run();
}
