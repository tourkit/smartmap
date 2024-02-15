#include "test.hpp"
#include "log.hpp"



// struct Foo { virtual Foo() { PLOGD << "foo"; } };
// struct Bar : Foo { Bar() { PLOGD << "bar"; Foo(); } };



#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>




struct Foo{ 
    int x;
    Foo(int x = 1) : x(x) { PLOGD << "foo"<<x; }
    Foo(const Foo& copy) { x = 3; PLOGD << "COPY"; }
    };
struct Bar{};
struct Etc{};

struct Any{};

template <typename T>
struct X { 

    T* ptr; 

    std::type_index stored_type;

    X(T* ptr = nullptr) : ptr(ptr), stored_type(typeid(*this->ptr)) { } 

    X<Any>* get() { return (X<Any>*)this; }

    std::string type() { return stored_type.name(); }
};

Test::Test(){

    auto xa = X<Foo>(new Foo(2));

    Foo f2(*xa.ptr);

    PLOGD << f2.x<<"!!!!";


    
    // auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    // obj->s->addComp({"Position"});

    // obj->push();

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    // PLOGD << "tete";


}