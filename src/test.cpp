#include "test.hpp"
#include "log.hpp"
#include "node.hpp"
#include "directory.hpp"
#include <cstring>
#include <typeinfo>
#include <functional>
#include <vector>

#include "shader.hpp"


Test::Test(){

struct Foo {

    std::string name;

    std::function<void(Foo*)> dtor;

    std::vector<Foo*> childrens;
    Foo* parent = nullptr;

    void update() {}
    void add(Foo* f) { childrens.push_back(f); f->parent = this; }
    void remove(Foo* f) { childrens.erase(std::remove(childrens.begin(), childrens.end(), f), childrens.end()); update(); } 

    Foo(std::string name = "foo", std::function<void(Foo*)> dtor = nullptr) : name(name), dtor(dtor) { PLOGD << name; }

    virtual ~Foo() { 

        for (auto c : childrens) delete c;
        PLOGD << "FOO";

        if (parent) parent->remove(this); 

        if (dtor) dtor(parent); 
    }

};

struct Bar : Foo { 

    Bar() : Foo("bar",[](Foo* dtor){ PLOGD << "BAR-"<< dtor->name; }) {  }

};

{

    // Foo f;

    // f.add(new Bar());

}



 
}