/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype

// vlayer effector

// grid / quantity 


// ----


#include "engine.hpp"
#include "src/freetype.hpp"
#include "texture.hpp"

int main() {

    engine.init();

    Freetype ft("c", 100);

    engine.tree->addOwnr<Texture>(ft.buffer, ft.width, ft.height, 0, 1, GL_RGB8,GL_RGBA);

    logger.cout(Sev::warning);

    engine.open("project.json");

    engine.run();

}
