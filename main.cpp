/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boi/lerplate.hpp"
// #include "debug.hpp"
// Debug debug;

// SmartMap& sm = SmartMap::getInstance();

#include "component.hpp"

int main() { 

    Component::init();

    for (auto c : Component::pool) {
        
        std::cout << c->name << std::endl;
        for (auto m : c->members) std::cout << " - " << m.name << " (" << m.range_from << "," << m.range_to << ")" << std::endl;
        
    }

}






