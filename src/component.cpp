#include "component.hpp"

Components::Components() {

        Component::pool.resize(0);

        Component::create("RGBA")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
            .member<float>("alpha")
        ;
        Component::create("RGB")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
        ;
        Component::create("Opacity")
            .member<float>("alpha")
        ;

        Component::create("Position")
            .member<float>("x").range(-1,1)
            .member<float>("y").range(-1,1)
        ;
        Component::create("Position3D")
            .member<float>("x")//.range(-1,1)
            .member<float>("y")//.range(-1,1)
            .member<float>("z")//.range(-1,1)
        ;
        Component::create("U32Vec3")
            .member<uint32_t>("x")//.range(-1,1)
            .member<uint32_t>("y")//.range(-1,1)
            .member<uint32_t>("z")//.range(-1,1)
        ;
        Component::create("UV")
            .member<float>("x")
            .member<float>("y")
        ;

        Component::create("Size")
            .member<float>("x")
            .member<float>("y")
        ;
        
        Component::create("pos")
            .member<glm::vec2>()
            .range(-1,1)
        ;
        Component::create("size")
            .member<glm::vec2>()
        ;
        Component::create("Gobo")
            .member<float>("ID")
            .member<float>("FX1")
            .member<float>("FX2")
            .member<float>("FX3")
        ;

        Component::create("Orientation")
            .member<float>("angle").range(-3.14159265359,3.14159265359)
        ;

        Component::create("Feedback")
            .member<float>("quantity")
        ;
        Component::create("Strobe")
            .member<float>("speed")
        ;
        Component::create("float")
            .member<float>("value")
        ;
        Component::create("char")
            .member<char>("value")
        ;
        Component::create("2chars")
            .member<uint8_t>("char1")
            .member<uint8_t>("char2")
        ;
        Component::create("Vec2")
            .member<float>("x")
            .member<float>("y")
        ;
        Component::create("Ratio")
            .member<float>("value")
        ;
        Component::create("int")
            .member<uint32_t>("value")
        ;
        Component::create("Vertex")
            .member<uint32_t>("id")
        ;
        Component::create("DimentionsUI32")
            .member<uint32_t>("width")
            .member<uint32_t>("height")
        ;
        Component::create("Resolution")
            .member<uint32_t>("width")
            .member<uint32_t>("height")
        ;
        Component::create("Dimentions")
            .member<float>("width")
            .member<float>("height")
        ;
        Component::create("ID")
            .member<uint32_t>("value")
        ;
        Component::create("Offset")
            .member<uint32_t>("value")
        ;

        auto &dmx = Component::create("DMX");
        for (size_t i = 0; i < 512; i++) dmx.member<uint8_t>("chan "+std::to_string(i));

}