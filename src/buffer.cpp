#include "buffer.hpp"
#include "widgets/buffer.hpp"
        
        int Buffer::Object::create() {
            
            quantity+= 1;
            buffer->updateBuffer();
            return components.size()-1;
        }

        void Buffer::Object::addComponent(std::string component) { 

            components.push_back(Component::id(component.c_str())); 

            size += Component::id(component.c_str())->size;

        }

        void *Buffer::Object::data() { return (void*)&buffer->data[offset]; }

        void Buffer::Object::set(const char* name, void* data, int id) { 

            Component *comp = nullptr;

            int offset = this->offset;

            for (auto &c:components) { 
                
                if (!strcmp(c->name.c_str(),name)) {

                    comp = c;
                    break;

                }

                offset += c->size;
            
            }

            if (comp) { memcpy(&buffer->data[size*id+offset], data, comp->size); }
            else{ std::cout << name << " does not recall" << std::endl; }

        }

        void Buffer::Object::set(int member_id, void* data, int obj_id) { 

            Component *comp = components[member_id];

            int offset = this->offset;
            for (size_t i = 0; i < member_id; i++) offset += components[i]->size;
           
            if (comp) memcpy(&buffer->data[size*obj_id+offset], data, comp->size);

        }



    Buffer::Object *Buffer::add(std::string name, std::vector<std::string> components, int quantity) { 

        objects.push_back({name, {}, quantity}); 

        for (auto comp : components) objects.back().addComponent(comp);
        
        updateBuffer(); 

        objects.back().buffer = this;

        return &objects.back();

    }

    void Buffer::reset() { objects.resize(0); data.resize(0); }

    void Buffer::updateBuffer() {

        // std::vector<std::vector<char>> datas;

        // for (auto &obj:objects) { 
            
        //     datas.resize(datas.size()+1);

        //     for (auto comp:obj.components) { 
                
        //         datas.back().resize(datas.size()  + comp->size*obj.quantity); 
                
        //     }

        //     memcpy(&datas.back()[0],&data[obj.offset],datas.back().size());

        // }

        int size = 0;
        // for (auto d:datas) { size += d.size(); }
        for (auto &obj:objects) { for (auto comp:obj.components) { size += comp->size*obj.quantity; } }
        data.resize(size);
        memset(&data[0],0,data.size());

        // int max_copy = tdata.size();
        // if (size < max_copy) max_copy = size;

        // memcpy(&data[0], &tdata[0], max_copy);

        int offset = 0;
        for (auto &obj:objects) {
            
            obj.offset = offset;

            offset += obj.size*obj.quantity;
            
        }

        callback();

    }

    Buffer::Buffer(std::string name) : name(name) { pool.push_back(this);  objects.reserve(10); BufferWidget::updateBufferList(); }