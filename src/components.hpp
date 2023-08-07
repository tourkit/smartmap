#pragma once
#include "reflection.hpp"


struct Components {

    Components() {
        
        Reflection::component("DMX Attribute")
            .member<uint8_t>("combining")
        ;

        Reflection::component("RGBA")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
            .member<float>("alpha")
        ;

        Reflection::component("Position")
            .member<float>("x").range(-1,1)
            .member<float>("y").range(-1,1)
        ;

        std::cout << "ooooo" << std::endl;

    };

};
