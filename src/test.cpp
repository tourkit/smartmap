#include "test.hpp"
#include "log.hpp"
#include "node.hpp"
#include "directory.hpp"
#include <cstring>
#include <typeinfo>
struct Foo {

    Foo() { }

    ~Foo() { PLOGD << "FOO"; }

    // virtual void gogo() {}
};

struct Bar : Foo {

    ~Bar()  { PLOGD << "BAR"; }

};


Test::Test(){

    auto b = new Bar();
    delete b;

    // Node tree = Node{"Tree"};

    // for (int i = 0; i < 3; i++) {

    //     tree.add(new Node{std::to_string(i)}); 

    // }


}