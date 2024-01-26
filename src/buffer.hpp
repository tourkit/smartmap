#pragma once

#include <cstring>
#include <vector>
#include <functional>

#include "component.hpp"
#include "node.hpp"

struct Buffer : Node {

    struct Struct : Node {

        struct Entry { 
            
            int id;
            Struct* obj;
            
            template <typename T>
            void set(int member_id, T data) { 

                auto comp = ((Ptr<Component>*)obj->childrens[member_id])->ptr;

                int buffer_offset = obj->buffer_offset;
                for (size_t i = 0; i < member_id; i++) buffer_offset += ((Ptr<Component>*)obj->childrens[i])->ptr->size;
            
                if (comp) memcpy(&obj->buffer->data[obj->byte_size*id+buffer_offset], (void*)&data, comp->size);                

            }

        };


        Struct(std::string name, std::vector<std::string> components = {}, int quantity = 1);

        int reserved , quantity = 0, byte_size = 0, buffer_offset = 0;

        void addComponent(std::string component);
    
        Node* add(Node* node) override;

        void editor() override;
        
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
    
    Node* add(Node* node) override;

    void reset();

    void update() override;
    void editor() override;

    void updateBuffer();
    // void changeObjQuantity();

    Buffer(std::string name = "Buffer");

    // ~Buffer();


private:

    int new_offset() { 
        
        int buffer_offset = 0;

        if (childrens.size()) {
            
            auto obj = ((Ptr<Struct>*)childrens.back())->ptr;
            buffer_offset = obj->buffer_offset+(obj->reserved*obj->byte_size);
        }

        return buffer_offset;

    }
};