#include "object.hpp"

#include <memory>
#include <cstring>

#include "struct.hpp"
#include "buffer.hpp"
#include "entry.hpp"

size_t Object::size() { return s->size * reserved; }

char *Object::data(size_t id) { return &buffer->data[offset + (s->size * id)]; }

Entry &Object::push() { 

    std::vector<char> data;
    data.resize(s->size);
    memset(&data[0],0,data.size());

    return push(&data[0]);

}

Entry &Object::push(void* data) { 

    auto backup_data = buffer->data;
    auto backup_objects = buffer->objects;

    reserved+=1;

    int offset = 0;

    for (auto obj : buffer->objects) {

        obj.offset = offset;

        offset += obj.size();
        
    }

    buffer->data.resize(offset);
    memset(&buffer->data[0],0,buffer->data.size()); 
    
    for (int obj_id = 0; obj_id < backup_objects.size(); obj_id++) {

        auto &o = backup_objects[obj_id];

        for (int entry_id = 0; entry_id < o.entrys.size(); entry_id++) {

            o.entrys[entry_id]->obj = &o;

            int comp_offset = 0;
            
            for (int comp_id = 0; comp_id < o.s->comps.size(); comp_id++) {

                int member_offset = 0;
                
                for (int member_id = 0; member_id < o.s->comps[comp_id]->members.size(); member_id++) {

                    auto offset = o.offset+(o.s->size*entry_id)+comp_offset+member_offset;

                    auto x = &backup_data[offset];


                    auto y = (*(*buffer)[obj_id]->entrys[entry_id])[comp_id][member_id].data;

                    memcpy(y,x,o.s->comps[comp_id]->members[member_id].size);

                    PLOGD << *(float*)y;
                    member_offset += o.s->comps[comp_id]->members[member_id].size;

                }

                comp_offset += o.s->comps[comp_id]->size;
            }
        }

    }

    int id = entrys.size();

    // memcpy(this->data()+s->size*id,data,s->size);

    // if (id>-1) memcpy(this->data()+s->size*id,data,s->size);

    // else memcpy(this->data(),data,s->size*reserved);

    // if (buffer->objects.size() && buffer->objects[0].entrys.size()) PLOGD << *(float*)(*(*buffer)[0]->entrys[0])[0][0].data;

    // if upscale rearange after rescale
    // if dowscale rearange before rescale

    entrys.push_back(new Entry{this,id});
    return *entrys.back();

}