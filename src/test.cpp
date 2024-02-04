#include "test.hpp"
#include "log.hpp"

#include <cstring>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <vector>
#include <map>


struct Foo {};

template <typename T>
struct Bar {

    static inline int x = 1;

};

Test::Test(){
    
    Bar<Foo> bar;

    Bar<Foo>::x;

    // can I access x with std::type_index of Foo (aka typeid(Foo)) ?

}