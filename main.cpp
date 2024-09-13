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


#include "engine.hpp"
#include "src/freetype.hpp"
#include "texture.hpp"
#include "builder.hpp"
#include "instance.hpp"
#include "effector.hpp"


int main() {


    logger.cout(Sev::warning);

    engine.init();

    Freetype ft("c", 100);

    engine.tree->addOwnr<Texture>(ft.buffer, ft.width, ft.height, 0, 1, GL_RGB8,GL_RGBA);

    engine.open("project.json");
    auto check_referimgs = engine.tree->find("Free Layer 1");



    engine.run();

}
