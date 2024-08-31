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

    
    auto vref = vlay.addEffector(uber.feedback);

    vlay_->addPtr<EffectorRef>(vref);

    auto lay_ = engine.stack->childrens[1];
    auto &lay = *lay_->is_a<Layer>();
    lay.m.name("testfb");

    lay.feedback = new Layer::Feedback(&lay);
    
    auto ref = lay.addEffector(lay.feedback);

    lay_->addPtr<EffectorRef>(ref);

    auto lay2_ = engine.stack->childrens[2];
    auto &lay2 = *lay2_->is_a<Layer>();
    lay2.m.name("testfb2");

    lay2.feedback = new Layer::Feedback(&lay2);
    
    auto ref2 = lay2.addEffector(lay2.feedback);

    lay2_->addPtr<EffectorRef>(ref2);

    engine.run();

    logger.cout(Sev::error);
}
