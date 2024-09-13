/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype


// grid / quantity 

// test ndi

// ----


#include "engine.hpp"
#include "src/freetype.hpp"
#include "texture.hpp"

int main() {

        // const char* zzz = "dsd\nfh\njk";

        // auto start = zzz;

        // while (const char * xxxxx = std::find(start, zzz+9, '\n')) {

        //     PLOGW << start;

        //     start = xxxxx;

        // }


    logger.cout(Sev::warning);

    engine.init();

    Freetype ft("c", 100);

    engine.tree->addOwnr<Texture>(ft.buffer, ft.width, ft.height, 0, 1, GL_RGB8,GL_RGBA);

    engine.open("project.json");

    engine.run();

}
