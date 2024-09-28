/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


#include "engine.hpp"
#include "layer.hpp"
#include "node.hpp"
#include "editor.hpp"



struct Nonode {

    std::string name;
    std::vector<Nonode*> childrens;

    Nonode* add(std::string name) {

        childrens.emplace_back(new Nonode{name});
        return childrens.back();

    }
};

void eachDF(std::vector<Nonode*> bucketin, std::function<bool(Nonode*)> cb, int depth = -1) {
    
    for (auto n : bucketin)
        if (!cb(n))
            return;

    std::vector<Nonode*> bucketout;

    for (auto n : bucketin) 
        for (auto c : n->childrens) 
            bucketout.push_back(c);
    
    if (bucketout.size() && depth != 0)
        eachDF(bucketout, cb, depth-1);

}

int main() {

    Nonode tree{"tree"};

    auto branhca = tree.add("brancha");

    auto a1 = branhca->add("a1");

    auto a2 = a1->add("a2");
    auto a3 = a2->add("a3");

    auto branhcb = tree.add("branchb");

    auto b1 = branhcb->add("b1");

    auto b2 = b1->add("b2");
    auto b3 = b2->add("b3");

    std::cout << "================================= \n";

    eachDF({&tree}, [](Nonode* n){ 
        
        // if (n.name == "a1"){
        //     std::cout << n.name << " FOUND!\n";
        //     return false;
        // }
        
        std::cout << n->name << "!\n";
        
        return true; 

    },2);



exit(0);
    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");  
    engine.window.size(100, 1);
    engine.gui(true);

    auto main_ = engine.tree->find("main")->allow<AnyNode>();

    auto argb_ = engine.tree->find("argb");
    auto rectangle_ = engine.tree->find("rectangle");
    auto quad_ = engine.tree->find("quad");


    main_->add(quad_);
    auto lay_ = main_->childrens.back();
    auto v1_ = lay_->childrens.back();
    v1_->add(argb_);
    v1_->add(rectangle_);
    auto v2_ = lay_->add(quad_);
    v2_->add(argb_);
    v2_->add(rectangle_);
    engine.gui_v->editors[0].get()->selected = lay_;
    engine.gui_v->editors[0].get()->locked = true;



    auto ubl_ = engine.tree->find("main")->addOwnr<UberLayer>();
    auto ubl = ubl_->is_a<UberLayer>();

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

    ubl->calc_matrice();

    engine.gui_v->editors[1].get()->selected = ubl_;
    engine.gui_v->editors[1].get()->locked = true;
    engine.run();

}
