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

    update();

    return &objects.back();

}

void Buffer::update() {

    auto backup_objects = objects; // should remode last ?

    auto backup_data = data;

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

            o.entrys[entry_id]->obj = &o;

            for (int comp_id = 0; comp_id < o.s->comps.size(); comp_id++) {

                for (int member_id = 0; member_id < o.s->comps[comp_id]->members.size(); member_id++) {

                    auto x = (*o.entrys[entry_id])[comp_id][member_id].data;

                    auto y = (*(*this)[obj_id]->entrys[entry_id])[comp_id][member_id].data;
                    
                    memcpy(y,x,o.s->comps[comp_id]->members[member_id].size);

                }
            }
        }

    }

}

char* Buffer::getEntry(Struct *s, int eq) {
    
    for (auto &obj : objects) {

        if (obj.s == s) {

            return obj.data() + obj.size() ;

        }

    }

    return nullptr;

} 