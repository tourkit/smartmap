#include "test.hpp"
#include "log.hpp"

Test::Test(){

    struct Foo{ 

        std::vector<Foo*> pool;
        
        int x; 

        Foo (int x = 5) : x(x) { pool.push_back(this); }

        Foo& y() { return *this; }
        // Foo y() { return this; }
        
    };

    struct Bar:Foo{};

    auto x = new Foo{6};

    x->y().x = 7;
    PLOGD<< x->x;

}