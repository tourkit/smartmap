#include "buffer.hpp"
#include "gui.hpp"

Buffer::Object::Object(std::string name, std::vector<std::string> components, int reserved)
    : Node(name), reserved(reserved) { 

    for (auto c : components) addComponent(c);

}

void Buffer::Object::addComponent(std::string component) { 

    Node::add(new Ptr<Component>(Component::id(component.c_str()))); 

    byte_size += Component::id(component.c_str())->size;

}


Node* Buffer::Object::add(Node* node) { 

    auto compptr = node->is_a<Ptr<Component>>();
    if (!compptr) return nullptr;
    auto comp = compptr->ptr;

    Node::add(node);

    return node;
}


void Buffer::Object::push(void *data, int quantity) {

    int first_byte = buffer_offset+(this->quantity*byte_size);

    memcpy(&buffer->data[first_byte], data, quantity*byte_size);

    this->quantity += quantity;

}

char *Buffer::Object::data() { return (char*)&buffer->data[buffer_offset]; }


Buffer::Buffer(std::string name) : Node(name) {  }

void Buffer::update() { 

    PLOGD<<"once";
    
}

Node* Buffer::add(Node* node) { 

    auto obj = node->is_a<Buffer::Object>();
    if (!obj) return nullptr;

    int new_offset_t = new_offset();

    Node::add(node);

    int size = 0;

    for (auto obj : childrens) { 
        for (auto comp : ((Object*)obj)->childrens) { 
            size += ((Ptr<Component>*)comp)->ptr->size*((Object*)obj)->reserved; 
        } 
    }

    data.resize(size);

    callback();

    obj->buffer_offset = new_offset_t;
    obj->buffer = this;

    // not enough need memory mgmt ( by splirt zhere last new offset)

    // update();

    return node;

 }

    void Buffer::editor() { 

        ImGui::Text(std::to_string(data.size()).c_str());

        for (auto c : childrens) { 

            auto obj = c->is_a<Object>();
            if (!obj) continue; 
            
            ImGui::Text(c->name.c_str()); ImGui::SameLine();
            ImGui::Text(std::to_string(obj->reserved).c_str());

            for (auto c : obj->childrens) { 

                auto compptr = c->is_a<Ptr<Component>>();
                if (!compptr) continue;
                auto comp = compptr->ptr;
                
                ImGui::Text(c->name.c_str()); ImGui::SameLine();
                ImGui::Text(std::to_string(comp->size).c_str());
            
            } 

        }

    }

    void Buffer::reset() { 
        // objects.resize(0); data.resize(0); 
        // 
        }


    void Buffer::updateBuffer() {

        // int size = 0;
        // for (auto &obj:objects) { for (auto comp:obj.components) { size += comp->size*obj.reserved; } }
        // data.resize(size);
        // memset(&data[0],0,data.size());


        // int buffer_offset = 0;
        // for (auto &obj:objects) {
            
        //     obj.buffer_offset = buffer_offset;

        //     buffer_offset += obj.byte_size*obj.reserved;
            
        // }

        // callback();

    }