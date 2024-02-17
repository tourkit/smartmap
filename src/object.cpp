#include "object.hpp"

#include <memory>
#include <cstring>

#include "struct.hpp"
#include "buffer.hpp"
#include "entry.hpp"

size_t Object::size() { return eq(reserved); }

char *Object::data(size_t id) { return &buffer->data[offset + eq(id)]; }

int Object::eq(size_t id) { return (s->size * id); }

Entry &Object::push() { 

    std::vector<char> data;
    data.resize(s->size);
    memset(&data[0],0,data.size());

    return push(&data[0]);

}

void Object::update(Buffer bkp) {

update2(bkp);

}

void Object::update2(Buffer bkp) {

    auto backup_data = bkp.data;
    auto backup_objects = bkp.objects;

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

            int comp_offset = 0;
            
            for (int comp_id = 0; comp_id < o.s->comps.size(); comp_id++) {
                
                int member_offset = comp_offset;
                
                for (int member_id = 0; member_id < o.s->comps[comp_id]->members.size(); member_id++) {

                    if (member_id >0) {


                        PLOGD<<"crash";

                    }
                           
                    auto offset = o.offset+(o.s->size*entry_id)+member_offset;

                    PLOGD<<backup_data.size();
                    auto x = &backup_data[offset];

                    PLOGD << o.s->comps[comp_id]->size;

                    memcpy((*buffer)[obj_id]->data(entry_id)+member_offset,x,o.s->comps[comp_id]->members[member_id].size);
                           
                    member_offset += o.s->comps[comp_id]->members[member_id].size;
                    if (member_id >0) {


                        PLOGD<<"smack";

                    }

                }

                // do for not backup members aswell

                comp_offset += o.s->comps[comp_id]->size;

            }

        }

    }

    int obj_offset = 0;
    for (auto& o : buffer->objects) {

        o.offset = obj_offset;
        obj_offset += o.size();

    }


}

Entry &Object::push(void* data) { 

    Buffer bkp = *buffer;

    reserved+=1;

    update(bkp);

    int id = entrys.size();

    memcpy(this->data()+eq(id),data,s->size); 

    entrys.push_back(new Entry{this,id});

    // shouldnt buffer->update() here ? if not find where does it happn now and if is logic ?
    
    return *entrys.back();

}   