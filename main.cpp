/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype


// ----

// quest ce quio updqte les tex dans editor::effector qui supdate pas dans editor::shaderpprogram

// cb de fois run feedback::body dans le shader


#include "effector.hpp"
#include "engine.hpp"
#include "layer.hpp"

int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("project.json");

    auto uber_ = engine.stack->childrens[0];
    auto &uber = *uber_->is_a<UberLayer>();
    uber.feedback = new UberLayer::Feedback(&uber);
    
    auto vlay_ = engine.stack->childrens[0]->childrens[0];
    auto &vlay = *vlay_->is_a<UberLayer::VirtualLayer>();

    
    auto ref = vlay.addEffector(uber.feedback);
    // lay.builder.build();

    vlay_->addPtr<EffectorRef>(ref);
    // PLOGW << lay.shader.frag.src;

    engine.run();

}
