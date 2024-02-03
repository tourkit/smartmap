#include "buffer.hpp" 

Struct Buffer::add(Struct s) { 

    int buffer_offset = 0;
    if (structs.size()) buffer_offset = structs.back().buffer_offset+(structs.back().reserved*structs.back().byte_size);

    structs.push_back(s); 

    int size = 0;
    for (auto &obj:structs) { for (auto comp:obj.childrens) { size += ((Component*)comp)->size*obj.reserved; } }
    data.resize(size);

    callback();

    structs.back().buffer_offset = buffer_offset;
    structs.back().buffer = this;

    return structs.back();

}

void Buffer::reset() { structs.resize(0); data.resize(0); }

Buffer::~Buffer() {

}

    void Buffer::updateBuffer() {

        
        // std::vector<std::vector<char>> datas;

        // int full_size = 0;   
        // for (auto &obj:structs) { 
            
        //     datas.resize(datas.size()+1);

        //     for (auto comp:obj.components) {
                
        //         datas.back().resize(datas.size()  + comp->size*obj.quantity); 

        //         full_size += comp->size*obj.reserved;
                
        //     }


        //     memcpy(&datas.back()[0],&data[obj.buffer_offset],datas.back().size());

        // }
        
        int size = 0;
        for (auto &obj:structs) { for (auto comp:obj.childrens) { size += ((Component*)comp)->size*obj.reserved; } } // ((Ptr<Component>*)obj->
        data.resize(size);
        memset(&data[0],0,data.size());

        // int max_copy = tdata.size();
        // if (size < max_copy) max_copy = size;

        // memcpy(&data[0], &tdata[0], max_copy);

        int buffer_offset = 0;
        for (auto &obj:structs) {
            
            obj.buffer_offset = buffer_offset;

            buffer_offset += obj.byte_size*obj.reserved;
            
        }

        callback();

    }

    Buffer::Buffer() {structs.reserve(10); }