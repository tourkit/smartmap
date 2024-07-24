/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "node.hpp"


    struct Bar { int x = 666;  };

    struct Foo : Bar { virtual void fuckthewholeworld() {} int y = 999; };

    struct Zi : Foo {  };
    
    struct  Test {};
    struct  Tree {};

int main() {


    NODE<Tree>::onadd<Bar>([](Node*_this,Node*node){

        std::cout << "bar: " << node->is_a<Bar>()->x << std::endl;

        return node;


    });

    NODE<Tree>::onadd<Foo>([](Node*_this,Node*node){

        std::cout << "foo: " << node->is_a<Foo>()->y << std::endl;
        
        return node;

    });

    NODE<Bar>::onchange([](Node*node,Bar*bar){

        std::cout << "barchange: "<< bar->x << std::endl;

    });

    // NODE<Bar>::onrun([](Node*node,Bar*bar){

    //     std::cout << "barrun: "<< std::endl;

    // });
    // NODE<Foo>::onrun([](Node*node,Foo*foo){

    //     std::cout << "foorun: "<< std::endl;

    // });


    Node x;


    auto tree = x.addOwnr<Tree>();

    NODE<Foo>::is_a<Bar>();

    NODE<Zi>::is_a<Foo>();

    auto n = tree->addOwnr<Foo>();

    n->trigchange();


}
