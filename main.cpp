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


    engine.init();

    Freetype ft("c", 100);

    engine.tree->addOwnr<Texture>(ft.buffer, ft.width, ft.height, 0, 1, GL_RGB8,GL_RGBA);

    engine.open("project.json");

    logger.cout(Sev::warning);

    auto argb = engine.tree->find("argb");
    auto quad1 = engine.tree->find("quad1");

        // auto effectable =  quad1->is_a<Effectable>();
        // auto effector =  argb->is_a<Effector>();

        // auto x = effectable->addEffector( effector );

        // auto n = quad1->addPtr<EffectorRef>(x);

        // node->referings.insert(n);

    quad1->add(argb);

    // freelayer need to be updated

    auto lst = Builder::unique_name({engine.dynamic_ubo});

    for (auto x : lst) 
        PLOGW << Builder::print_struct(x.first, lst);


    engine.run();

}
