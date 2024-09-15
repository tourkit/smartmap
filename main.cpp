

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




int main() {


    logger.cout(Sev::warning);

    Member buff("buff");
    buff.buffering(true);

    Member layer("layer");
    layer.quantity(2);
    buff.add(&layer);

    Member quad("quad");
    quad.quantity(2); 
    layer.add(&quad);

    Member argb("argb");
    argb.add<char>("ch").range(0,1,2); 

    Member ref("ref");
    ref.add(&argb);
    quad.add(&ref);

    Instance inst(buff);

    Instance refinst(inst);
    refinst[&layer][&quad][&ref].parent();

    // branchSTL (refinst.stl);

    inst.print();

}
