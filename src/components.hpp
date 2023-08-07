#pragma once
#include "reflection.hpp"


struct Components {

    struct RGBA { float red, green, blue, alpha; };
    struct Size2D { float width, height; };
    struct Position2D { float x, y; };

    struct Range { uint8_t from,to; };

    struct DMXAttribute { uint8_t combining; };

    Components() {
        

        Reflection::component<DMXAttribute>("DMX Attribute")
            .member<uint8_t>("combining")
        ;

        Reflection::component<RGBA>("RGBA")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
            .member<float>("alpha")
        ;

        Reflection::component<Position2D>("Position")
            .member<float>("x").range(-1,1)
            .member<float>("y").range(-1,1)
        ;

        std::cout << "ooooo" << std::endl;

    };

};
