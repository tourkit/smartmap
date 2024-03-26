/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include "engine.hpp"
#include "boilerplate.hpp"

#include "drawcall.hpp"

int main() { logger.cout(true); 
    
    engine.init(); 
    
    auto &dc = *engine.tree->addOwnr<DrawCall>()->active(true);

    engine.gui->editors.push_back(new EditorWidget());

    engine.run();
    
}