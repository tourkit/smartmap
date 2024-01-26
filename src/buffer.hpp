#pragma once

#include "pch.hpp"

#include "component.hpp"
#include "node.hpp"

struct Buffer : Node {

    struct Object : Node {

        struct Entry { 
            
            int id;
            Object* obj;
            
            template <typename T>
            void set(int member_id, T data) { 

                auto comp = ((Ptr<Component>*)obj->childrens[member_id])->ptr;

                int buffer_offset = obj->buffer_offset;
                for (size_t i = 0; i < member_id; i++) buffer_offset += ((Ptr<Component>*)obj->childrens[i])->ptr->size;
            
                if (comp) memcpy(&obj->buffer->data[obj->byte_size*id+buffer_offset], (void*)&data, comp->size);                

            }

        };


        Object(std::string name, std::vector<std::string> components = {}, int quantity = 1);

        int reserved , quantity = 0, byte_size = 0, buffer_offset = 0;

        void addComponent(std::string component);

        void push(void *data, int quantity = 1);

        char *data();

        Entry create() { 

            std::vector<char> data;
            data.resize(byte_size);
            push(&data[0]);
            return Entry{quantity-1, this};

        }

        Buffer* buffer;

    };

    std::vector<char> data;  
    
    std::function<void()> callback = [](){};

    Object *addObject(std::string name, std::vector<std::string> components = {}, int quantity = 1);
    
    Node* add(Node* node) override;

    void reset();

    void updateBuffer();
    // void changeObjQuantity();

    Buffer(std::string name = "Buffer");

    // ~Buffer();

};