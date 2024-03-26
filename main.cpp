/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "boilerplate.hpp"

#include "drawcall.hpp"

int main() { logger.cout(true); 
    
    engine.init(); 
    
    auto &dc = *engine.stack->addOwnr<DrawCall>();

    dc.add((*engine.models)[0])->add((*engine.effectors)[1]);

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
}