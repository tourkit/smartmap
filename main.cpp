/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"
#include "layer.hpp"

struct Foo{};
int main() {

    engine.init();

    logger.cout(Sev::warning);

    engine.open("./project.json");

    engine.tree->addOwnr<Foo>();//->name("dskhj");


    auto lay_ = engine.stack->addOwnr<Layer>(1920,1080);

    lay_->on(Node::CHANGE, [](Node* node){

        std::cout << "ooo";

    });

    lay_->add(engine.models->childrens[0])->add(engine.effectors->child("rectangle"));

    engine.run();

}
