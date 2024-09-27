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



    auto vl = ubl->addLayer(100, 100);
    vl.quantity(5);
    auto vl_ = ubl_->addPtr<UberLayer::VirtualLayer>(&vl);
    vl_->active(true);

    vl_->add(engine.tree->find("argb"));

    ubl->calc_matrice();
    ubl->upload();
    ubl->update_pv();

    ubl_->trig(Node::CHANGE);

    engine.run();

}
