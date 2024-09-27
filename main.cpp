/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


#include "engine.hpp"
#include "layer.hpp"

int main() {

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");  

    engine.tree->find("main")->allow<UberLayer>();
    auto ubl_ = engine.tree->find("main")->addOwnr<UberLayer>();

    auto ubl = ubl_->is_a<UberLayer>();


    auto argb_ = engine.tree->find("argb");

    auto &vl1 = ubl->addLayer(100, 100);
    vl1.quantity(5);
    auto vl1_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl1);
    vl1_->active(true);
    vl1_->add(argb_);

    auto &vl2 = ubl->addLayer(100, 100);
    vl2.quantity(2);
    auto vl2_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl2);
    vl2_->active(true);
    vl2_->add(argb_);

    ubl->calc_matrice();
    ubl->upload();
    ubl->update_pv();

    ubl_->trig(Node::CHANGE);

    engine.run();

}
