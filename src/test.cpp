#include "test.hpp"
#include "log.hpp"

Test::Test(){

    struct Foo{};
    struct Bar:Foo{};

    // PLOGD<< "TEST";

}