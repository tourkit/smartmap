#include "log.hpp" 
#include "buffer.hpp" 
#include "struct.hpp" 
#include "entry.hpp" 
#include <cstring>



Buffer::Buffer() { objects.reserve(1000); }

void Buffer::reset() { destroy(); }

void Buffer::destroy() { objects.resize(0); data.resize(0); }

Object* Buffer::addObj(Struct* s, int reserved) {

    if (objects.size()==999) PLOGW << "obj reserve reached";

    objects.push_back({s,reserved,this,data.size()});

    if (data.size()) update();

    return &objects.back();

}

Buffer Buffer::bkp(){

    //gloubiboulbakup

    std::vector<std::pair<Object*,Struct*>> list;

    for (auto obj : objects) {

        auto s = *obj.s;
        list.push_back({&obj,obj.s});
        for (auto i = 0; i < s.comps.size(); i++) s.comps[i] = new Component(*s.comps[i]);
        obj.s = &s;
        
    }

    Buffer bkp = *this;

    for (auto e : list) e.first->s = e.second;

    return bkp;

};

void Buffer::transpose(Buffer bkp) {

    auto backup_data = bkp.data;
    auto backup_objects = bkp.objects;

    int offset = 0;

    for (auto obj : objects) {

        obj.offset = offset;

        offset += obj.size();
        
    }

    data.resize(offset);

    memset(&data[0],0,data.size()); 
    
    for (int obj_id = 0; obj_id < backup_objects.size(); obj_id++) {

        auto &o = backup_objects[obj_id];

        for (int entry_id = 0; entry_id < o.entrys.size(); entry_id++) {

            int comp_offset = 0;
            
            for (int comp_id = 0; comp_id < o.s->comps.size(); comp_id++) {
                
                int member_offset = comp_offset;
                
                for (int member_id = 0; member_id < o.s->comps[comp_id]->members.size(); member_id++) {
                           
                    auto offset = o.offset+(o.s->size()*entry_id)+member_offset;

                    auto x = &backup_data[offset];

                    memcpy((*this)[obj_id]->data(entry_id)+member_offset,x,o.s->comps[comp_id]->members[member_id].size);
                           
                    member_offset += o.s->comps[comp_id]->members[member_id].size;

                }

                comp_offset += o.s->comps[comp_id]->size;

            }

        }

    }

    int obj_offset = 0;
    for (auto& o : objects) {

        o.offset = obj_offset;
        obj_offset += o.size();

    }

}