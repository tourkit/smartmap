/*

    SMARTMAP (beta)

                        */

#include "smartmap.hpp"
// #include "boi/lerplate.hpp"
// #include "debug.hpp"
// Debug debug;

// SmartMap& sm = SmartMap::getInstance();


#include "components.hpp"




int main() { 

    Components components;
    Reflection meta;

    for (auto c : Reflection::components) {
        
        std::cout << c.second.name << std::endl;
        for (auto m : c.second.members) std::cout << " - " << m.name << " (" << m.range_from << "," << m.range_to << ")" << std::endl;
        
    }

}






