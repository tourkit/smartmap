#pragma once

#include "component.hpp"
#include "struct.hpp"

struct Buffer {

    std::vector<Struct> structs;

    std::vector<char> data;  
    
    std::function<void()> callback = [](){};

    // void remove(Object *obj, std::vector<Component*> components) {  for (auto comp : components) { obj->components.push_back(comp); } resize(); }

    Struct add(Struct s);

    void reset();

    void updateBuffer();
    // void changeObjQuantity();

    Buffer();

    ~Buffer();

};