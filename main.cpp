/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "node.hpp"


struct Bar { int x = 666;  };

struct Foo : Bar { virtual void fuckthewholeworld() {} int y = 999; virtual ~Foo() { std::cout << "oooo" << 0 << "\n"; } };

struct Zi : Foo {  };

struct  Test { };

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

    NODE<Bar>::on(Node::CHANGE, [](Node*node,Bar*bar){

        std::cout << "barchange: "<< bar->x << std::endl;

    });
    NODE<Foo>::on(Node::DELETE, [](Node*node,Foo*foo){

        std::cout << "foodelete: "<< foo->y << std::endl;

    });

    Node x;

    Node &tree = *x.addOwnr<Tree>();

    NODE<Foo>::is_a<Bar>();

    NODE<Zi>::is_a<Foo>();

    Node &n = *tree.addOwnr<Foo>();

    n.trig(Node::CHANGE);

    delete &n;


}
