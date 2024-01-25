#include "test.hpp"
#include "log.hpp"
#include "node.hpp"
#include "directory.hpp"
#include <cstring>
#include <typeinfo>

#include "shader.hpp"


Test::Test(){
struct Foo {

    int x;

    Foo(int x) : x(x) { }

    virtual ~Foo() {  }

};

struct Bar : Foo { int y = 9; };


// can I update futurbar to Bar type ( with correctly allocated memopry) without running any ctor ?

 
}