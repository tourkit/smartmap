/*

    SMARTMAP (beta)

                        */
 
#include "node.hpp"

int main() { 
    
    logger.cout();

    Node tree;
    tree.addOwnr<Node>("x")->addOwnr<Node>("y");
    tree.addOwnr<Node>("a");
    tree.addOwnr<Node>("1")->addOwnr<Node>("2")->addOwnr<Node>("3");

    auto x = tree.child("x::y");

    if (x) PLOGW<< "SISISIISISIS";
    else PLOGW<< "NONONONON";
 
}