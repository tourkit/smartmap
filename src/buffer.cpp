#include "buffer.hpp"
#include "widgets/buffer.hpp"
        


        void Buffer::Object::push(void *data, int quantity) {

            int first_byte = buffer_offset+(this->quantity*byte_size);

            memcpy(&buffer->data[first_byte], data, quantity*byte_size);

            this->quantity += quantity;

        }


        void Buffer::Object::addComponent(std::string component) { 

            components.push_back(Component::id(component.c_str())); 

            byte_size += Component::id(component.c_str())->size;

        }

        char *Buffer::Object::data() { return (char*)&buffer->data[buffer_offset]; }

        void Buffer::Object::set(const char* name, void* data, int id) {  // UNUSESD

            Component *comp = nullptr;

            int buffer_offset = this->buffer_offset;

            for (auto &c:components) { 
                
                if (!strcmp(c->name.c_str(),name)) {

                    comp = c;
                    break;

                }

                buffer_offset += c->size;
            
            }

            if (comp) { memcpy(&buffer->data[byte_size*id+buffer_offset], data, comp->size); }
            else{ PLOGW << name << " does not recall"; }

        }

        void Buffer::Object::set(int member_id, void* data, int obj_id) {  // UNUSESD

            Component *comp = components[member_id];

            int buffer_offset = this->buffer_offset;
            for (size_t i = 0; i < member_id; i++) buffer_offset += components[i]->size;
           
            if (comp) memcpy(&buffer->data[byte_size*obj_id+buffer_offset], data, comp->size);

        }



    Buffer::Object *Buffer::add(std::string name, std::vector<std::string> components, int reserved) { 

        int buffer_offset = 0;
        if (objects.size()) buffer_offset = objects.back().buffer_offset+(objects.back().reserved*objects.back().byte_size);

        objects.push_back({name, {}, reserved}); 

        for (auto comp : components) objects.back().addComponent(comp);

        
        int size = 0;
        for (auto &obj:objects) { for (auto comp:obj.components) { size += comp->size*obj.reserved; } }
        data.resize(size);

        callback();

        objects.back().buffer_offset = buffer_offset;
        objects.back().buffer = this;

        return &objects.back();

    }

    void Buffer::reset() { objects.resize(0); data.resize(0); }

    void Buffer::updateBuffer() {

        
        // std::vector<std::vector<char>> datas;

        // int full_size = 0;   
        // for (auto &obj:objects) { 
            
        //     datas.resize(datas.size()+1);

        //     for (auto comp:obj.components) {
                
        //         datas.back().resize(datas.size()  + comp->size*obj.quantity); 

        //         full_size += comp->size*obj.reserved;
                
        //     }


        //     memcpy(&datas.back()[0],&data[obj.buffer_offset],datas.back().size());

        // }
        
        int size = 0;
        for (auto &obj:objects) { for (auto comp:obj.components) { size += comp->size*obj.reserved; } }
        data.resize(size);
        memset(&data[0],0,data.size());

        // int max_copy = tdata.size();
        // if (size < max_copy) max_copy = size;

        // memcpy(&data[0], &tdata[0], max_copy);

        int buffer_offset = 0;
        for (auto &obj:objects) {
            
            obj.buffer_offset = buffer_offset;

            buffer_offset += obj.byte_size*obj.reserved;
            
        }

        callback();

    }

    Buffer::Buffer(std::string name) : name(name) { pool.push_back(this);  objects.reserve(10); BufferWidget::updateBufferList(); }