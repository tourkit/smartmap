#pragma once

#include "pch.hpp"

#include "component.hpp"
#include "gui.hpp"

struct Buffer {

    struct Object {

        std::string name;

        std::vector<Component*> components;

        int quantity,size = 0, offset = 0;
        
        int create();

        void add(std::string component);

        void *data();

        void set(const char* name, void* data, int id = 0);

        void set(int member_id, void* data, int obj_id = 0);

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

    Buffer(std::string name = "Buffer");

};