/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


#include "engine.hpp"
#include "layer.hpp"
#include "node.hpp"

int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");  

    auto main_ = engine.tree->find("main")->allow<AnyNode>();

    auto ubl_ = engine.tree->find("main")->addOwnr<UberLayer>();
    auto ubl = ubl_->is_a<UberLayer>();

    auto argb_ = engine.tree->find("argb");
    auto quad_ = engine.tree->find("quad");

    // auto v1 = ubl->addModel(quad_->is_a<File>());
    main_->add(quad_);
    auto lay_ = main_->childrens.back();
    auto v1_ = lay_->childrens.back();
    v1_->add(argb_);
    auto v2_ = lay_->add(quad_);
    v2_->add(argb_);

    auto &vl1 = ubl->addLayer(100, 1);
    vl1.quantity(5);
    auto vl1_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl1);
    vl1_->active(true);
    vl1_->add(argb_);

    auto &vl2 = ubl->addLayer(100, 1);
    vl2.quantity(2);
    auto vl2_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl2);
    vl2_->active(true);
    vl2_->add(argb_);

    engine.run();

}
