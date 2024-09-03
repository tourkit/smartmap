#include "engine.hpp"
#include "utils.hpp"

struct Foo{};

int main() {

    Node n;

    NODE<Node>::onadd<AnyType>([](Node*_this,Node*node){ return node; });
    
    auto x = n.addOwnr<Foo>();

    PLOGE << x->name();
    
}

