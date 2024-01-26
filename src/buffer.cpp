#include "buffer.hpp"
#include "gui.hpp"

Buffer::Object::Object(std::string name, std::vector<std::string> components, int reserved)
    : Node(name), reserved(reserved) { 

    for (auto c : components) add(Component::id(c.c_str()));

}

Node* Buffer::Object::add(Node* node) { 

    auto comp = node->is_a<Component>();
    if (!comp) return nullptr;

    Node::add(new Ptr<Component>(comp));
    byte_size += comp->size;

    return node; 

}

void Buffer::Object::editor() {


 }


void Buffer::Object::push(void *data, int quantity) {

    int first_byte = buffer_offset+(this->quantity*byte_size);

    memcpy(&buffer->data[first_byte], data, quantity*byte_size);

    this->quantity += quantity;

}

char *Buffer::Object::data() { return (char*)&buffer->data[buffer_offset]; }


Buffer::Buffer(std::string name) : Node(name) {  }

void Buffer::update() { 

    int size = 0;

    for (auto obj : childrens) { 
        for (auto comp : ((Object*)obj)->childrens) { 
            size += ((Ptr<Component>*)comp)->ptr->size*((Object*)obj)->reserved; 
        } 
    }

    data.resize(size);
    
    // TODO: not enough need memory mgmt ( by splirt zhere last new offset)

}

Node* Buffer::add(Node* node) { 

    auto obj = node->is_a<Buffer::Object>();
    if (!obj) return nullptr;

    int new_offset_t = new_offset();

    Node::add(node);

    callback();

    obj->buffer_offset = new_offset_t;
    obj->buffer = this;


    // update();

    return node;

 }

void Buffer::editor() { 

    if (!childrens.size()) return;
    auto obj = childrens[0]->is_a<Buffer::Object>();
    if (!obj) return;

    int uniform_offset = obj->buffer_offset;

    int elem_current = 0;
    if (ImGui::SliderInt("current##uibocurrent", &elem_current,0,obj->reserved-1)) {}

    for (auto c:obj->childrens) {

        auto compptr = c->is_a<Ptr<Component>>();
        if (!compptr) continue;
        auto comp = compptr->ptr;
        
        // ImGui::Text((std::to_string(elem_current)+" - "+std::to_string(obj.size)).c_str());
        ImGui::SeparatorText(comp->name.c_str());
        
        for (auto m:comp->members) {

            float *value = (float*)&data[uniform_offset+(elem_current*obj->byte_size)];

            if (ImGui::SliderFloat((m.name+"##"+comp->name+m.name+std::to_string(uniform_offset)).c_str(), value, m.range_from, m.range_to)) { 
                
                // ubo->update(); 

            }

            uniform_offset += m.size; 
        }

        // break; 

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