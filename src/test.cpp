#include "test.hpp"
#include "log.hpp"
#include "node.hpp"
#include "directory.hpp"
#include <cstring>
#include <typeinfo>
#include <functional>
#include <vector>

#include "engine.hpp"
#include "buffer.hpp"



struct Foo {

    Foo() { PLOGD << "foo"; }

};


static std::vector<Foo> foos;

static Foo& add(Foo f) { foos.push_back(f); return foos.back(); }

Test::Test(){


    Foo &f = add(Foo());


    auto &buffer = *(Buffer*)Engine::getInstance().tree.add(new Buffer);

    buffer.addObject("TOUT",{"float"},1);



 
}