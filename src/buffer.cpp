#include "log.hpp" 
#include "buffer.hpp" 
#include "engine.hpp" 
#include "struct.hpp" 
#include "instance.hpp" 
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

Buffer* Buffer::bkp(){ //gloubiboulbakup

    if (bkps.find(this) != bkps.end()) {

        for (auto &bkpobj : bkps[this].objects) {

            for (auto c : bkpobj.s->comps) delete c; // struct and comp should be smarter poiters wacko

            delete bkpobj.s;
            
        }

    }

    bkps[this] = *this;
    auto bkp = &bkps[this];
    
    for (auto &bkpobj : bkp->objects) {

        bkpobj.s = new Struct(*bkpobj.s);

        for (auto i = 0; i < bkpobj.s->comps.size(); i++) bkpobj.s->comps[i] = new Component(*bkpobj.s->comps[i]);

        bkpobj.buffer = bkp;
        
    }

    return bkp;

};

struct LeIntATontonRocky { int value = 0; };

void Buffer::remap(Buffer* bkp) {

    // reset each buffer::objects offsets

    int offset = 0;

    for (auto &obj : objects) {

        obj.offset = offset;

        offset += obj.size();
        
    }

    // resize full  buffer

    data.resize(offset);

    memset(&data[0],0,data.size()); 

    //go thought bkp objs

    for (int obj_id = 0; obj_id < bkp->objects.size(); obj_id++) {

        auto &bkpobj = bkp->objects[obj_id];

        //find corresponding new obj

        Object *newobj = nullptr;
        for (auto &o : objects) if (!strcmp(bkpobj.s->name.c_str(),o.s->name.c_str())) newobj = &o;
        if (!newobj) { PLOGW << bkpobj.s->name; continue; }  // if not, next
        
        //go thought bkp instances // repeat from comp per instance_id
        
        for (int instance_id = 0; instance_id < bkpobj.instances.size(); instance_id++) {

            std::map<std::string,LeIntATontonRocky> member_count;
    
            int bkpcomp_offset = 0;

            //go thought bkp comps
            
            for (int comp_id = 0; comp_id < bkpobj.s->comps.size(); comp_id++) {

                auto bkpcomp = bkpobj.s->comps[comp_id];

                //find corresponding new comp
                
                Component* newcomp = nullptr;                   

                int newmember_count = 0;
                int newcomp_offset = 0;

                for (auto c : newobj->s->comps) {

                    if (!strcmp(bkpcomp->name.c_str(),c->name.c_str())) { 
                        
                        if (newmember_count == member_count[c->name].value) {
                            
                            newcomp = c; 
                            
                            member_count[bkpcomp->name].value++;

                            break; 
                        
                        }
                        
                        newmember_count++;
                        
                    }

                    newcomp_offset+= (bkpobj.s->is_striding ? nextFactor(c->size,16) : c->size);
                }

                if (!newcomp) { PLOGW << newobj->s->comps[comp_id]->name; continue; }

                // if (member.size == 1 and member[0].name.lenght == 0 and newmember_count == 1) // could be for mono member comps

                //go thought bkp members

                int bkpmember_offset = 0;
                
                for (int member_id = 0; member_id < bkpcomp->members.size(); member_id++) {

                    auto &bkpmember = bkpcomp->members[member_id];

                    Member* newmember = nullptr;

                    int newmember_offset = 0;

                    for (auto &m : newcomp->members) {

                        if (!strcmp(m.name.c_str(),bkpmember.name.c_str())) { newmember = &m; break; }

                        newmember_offset+= m.size;
                        
                    }

                    if (!newmember) { PLOGV << bkpmember.name; continue; }
                    static float wer = 9;
                    memcpy(
                        newobj->data(instance_id)+newcomp_offset+newmember_offset, 
                        bkpobj.data(instance_id)+bkpcomp_offset+bkpmember_offset, 
                        bkpobj.s->comps[comp_id]->members[member_id].size 
                    );
  
                    bkpmember_offset += bkpobj.s->comps[comp_id]->members[member_id].size;

                }

                bkpcomp_offset += bkpcomp->size; 

            }

        }

    }

}