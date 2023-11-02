#pragma once

#include "pch.hpp"

#include "component.hpp"
#include "gui.hpp"

struct Buffer {

    
    struct Object {

        struct Entry { 
            
            int id;
            Object* obj;
            
            template <typename T>
            void set(int member_id, T data) { 

                Component *comp = obj->components[member_id];

                int buffer_offset = obj->buffer_offset;
                for (size_t i = 0; i < member_id; i++) buffer_offset += obj->components[i]->size;
            
                if (comp) memcpy(&obj->buffer->data[obj->byte_size*id+buffer_offset], (void*)&data, comp->size);                

            }

        };

        std::string name;

        std::vector<Component*> components;

        int reserved , quantity = 0, byte_size = 0, buffer_offset = 0;

        void addComponent(std::string component);

        void push(void *data, int quantity = 1);

        void *data();

        void set(const char* name, void* data, int id = 0); // UNUSESD

        void set(int member_id, void* data, int obj_id = 0); // UNUSESD

        Entry create() { 

            std::vector<char> data;
            data.resize(byte_size);
            push(&data[0]);
            return Entry{quantity-1, this};

        }


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