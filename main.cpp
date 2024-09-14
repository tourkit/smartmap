

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
#include "src/freetype.hpp"
#include "texture.hpp"
#include "builder.hpp"
#include "instance.hpp"
#include "buffer.hpp"


int main() {

    logger.cout(Sev::warning);


    engine.init();
    engine.open("project.json");

    auto dc = engine.tree->find("Stack")->childrens[0]->is_a<DrawCall>();
;
    // dc->shader.sendUniform()
    engine.dynamic_ubo->bind(dc->shader);

    logger.cout(Sev::verbose);
    engine.run();
    logger.cout(Sev::warning);
}
