/*

    SMARTMAP (beta)

                        */

// #include "engine.hpp"
#include "node.hpp"

int main() { 
    
    // auto &engine = Engine::getInstance();

    // engine.init();

    // engine.open("project.json");

    // Test test;

    // engine.run();
    
    logger.cout();

    Node tree;
    tree.addOwnr<Node>("x")->addOwnr<Node>("y");
    tree.addOwnr<Node>("a");
    tree.addOwnr<Node>("1")->addOwnr<Node>("2")->addOwnr<Node>("3");

    PLOGW<< "hey";
 
}