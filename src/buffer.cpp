#include "buffer.hpp"
#include "gui.hpp"
#include "struct.hpp"
#include "component.hpp"


Buffer::Buffer(std::string name) : Node(name) {  }

void Buffer::update() { 

    int size = 0;

    for (auto obj : childrens) { 
        for (auto comp : ((Struct*)obj)->childrens) { 
            size += ((Ptr<Component>*)comp)->ptr->size*((Struct*)obj)->reserved; 
        } 
    }

    data.resize(size);
    
    // TODO: not enough need memory mgmt ( by splirt zhere last new offset)

}


Node* Buffer::add(Node* node) { 

    auto obj = node->is_a<Struct>();
    if (!obj) return nullptr;

    int new_offset_t = new_offset();

    Node::add(node);

    callback();

    obj->buffer_offset = new_offset_t;
    obj->buffer = this;


    // update();

    return node;

 }
int Buffer::new_offset() { 
        
        int buffer_offset = 0;

        if (childrens.size()) {
            
            auto obj = ((Ptr<Struct>*)childrens.back())->ptr;
            buffer_offset = obj->buffer_offset+(obj->reserved*obj->byte_size);
        }

        return buffer_offset;

    }
void Buffer::editor() { 

    if (!childrens.size()) return;
    auto obj = childrens[0]->is_a<Struct>();
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