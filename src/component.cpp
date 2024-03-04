#include "component.hpp"

Components::Components() {

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

    Component::create<uint32_t>("ID");

    Component::create<uint32_t>("Vertex");

    Component::create<glm::vec2>("pos").range(-1,1);

    Component::create<glm::vec2>("size");

    Component::create<float>("float");

    Component::create<char>("char");

    Component::create<glm::vec2>("vec2");

    Component::create<uint32_t>("int");

}