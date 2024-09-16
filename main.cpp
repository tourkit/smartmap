

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


#include "boilerplate.hpp"
#include "drawcall.hpp"
#include "editor.hpp"
#include "engine.hpp"
#include "builder.hpp"
#include "layer.hpp"
#include "gui.hpp"
#include "member.hpp"



// fix Member::adding all occurence set default  ..


int main() {

    // Boilerplate::Init();

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");

    // auto lay = engine.stack->childrens[0]->is_a<Member>()->instance;

    engine.run();

    logger.cout(Sev::warning);


    // Member buff("buff");
    // buff.buffering(true);

    // Member layer("layer");
    // layer.quantity(3);
    // buff.add(&layer);

    // Member quad("quad");
    // quad.quantity(2); 
    // layer.add(&quad);

    // Member argb("argb");
    // argb.add<char>("ch").range(0,1,2); 
    // Member otro("otro");
    // otro.add<float>("ch").range(0,1,3); 

    // Member ref("ref");
    // ref.add(&argb);
    // quad.add(&ref);

    // Member otroref("otroref");
    // otroref.add(&otro);
    // quad.add(&otroref);

    // Instance inst(buff);

    // Instance refinst(inst);
    // refinst[&layer][&quad][&ref];//.parent();


    // inst.print();

}
