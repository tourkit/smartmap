#pragma once

#include "pch.hpp"

#include "component.hpp"
#include "gui.hpp"

struct Buffer {

    struct Object {

        std::string name;

        std::vector<Component*> components;

        int reserved , quantity = 0, byte_size = 0, buffer_offset = 0;

        void addComponent(std::string component);

        void push(void *data, int quantity = 1);

        void *data();

        void set(const char* name, void* data, int id = 0); // UNUSESD

        void set(int member_id, void* data, int obj_id = 0); // UNUSESD

        Buffer* buffer;

    };

    std::string name;

    std::vector<Object> objects;

    std::vector<char> data;  

    static inline std::vector<Buffer*> pool;
    
    std::function<void()> callback = [](){};

    // void remove(Object *obj, std::vector<Component*> components) {  for (auto comp : components) { obj->components.push_back(comp); } resize(); }

    Object *add(std::string name, std::vector<std::string> components = {}, int quantity = 1);

    void reset();

    void updateBuffer();
    // void changeObjQuantity();

    Buffer(std::string name = "Buffer");

};