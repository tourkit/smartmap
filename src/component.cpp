#include "component.hpp"

Components::Components() {

        Component::pool.resize(0);


        Component::create("Position")
            .member<float>("x").range(-1,1)
            .member<float>("y").range(-1,1)
        ;

        Component::create("UV")
            .member<float>("x")
            .member<float>("y")
        ;

        Component::create("Size")
            .member<float>("x")
            .member<float>("y")
        ;
        Component::create("ID")
            .member<uint32_t>("value")
        ;
        Component::create("Vertex")
            .member<uint32_t>("id")
        ;
        
        Component::create("pos")
            .member<glm::vec2>()
            .range(-1,1)
        ;
        Component::create("size")
            .member<glm::vec2>()
        ;
        Component::create("float")
            .member<float>("value")
        ;
        Component::create("char")
            .member<char>("value")
        ;
        Component::create("vec2")
            .member<glm::vec2>()
        ;
        Component::create("int")
            .member<uint32_t>("value")
        ;

        auto &dmx = Component::create("DMX");
        for (size_t i = 0; i < 512; i++) dmx.member<uint8_t>("chan "+std::to_string(i));

}