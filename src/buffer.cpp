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

    Buffer bkp = *this;;

    for (auto &bkpobj : bkp.objects) {

        bkpobj.s = new Struct(*bkpobj.s);

        for (auto i = 0; i < bkpobj.s->comps.size(); i++) bkpobj.s->comps[i] = new Component(*bkpobj.s->comps[i]);
        
    }

    PLOGD << "Backup old comps COMPONENTS";

    return bkp;

};

void Buffer::transpose(Buffer bkp) {

    int offset = 0;

    for (auto obj : objects) {

        obj.offset = offset;

        offset += obj.size();
        
    }

    data.resize(offset); // pk pas pris en compte nouveau member ds obj.s->comps[x] ?

    memset(&data[0],0,data.size()); 
    
    for (int obj_id = 0; obj_id < bkp.objects.size(); obj_id++) {

        auto &bkpobj = bkp.objects[obj_id]; // check for names ?

        Object *newobj = nullptr;
        for (auto &o : objects) if (!strcmp(bkpobj.s->name.c_str(),o.s->name.c_str())) newobj = &o;
        if (!newobj) { PLOGW << "couldntfind" << bkpobj.s->name; continue; }

        for (int entry_id = 0; entry_id < bkpobj.entrys.size(); entry_id++) {

            int bkpcomp_offset = 0;
            int newcomp_offset = 0;
            
            for (int comp_id = 0; comp_id < bkpobj.s->comps.size(); comp_id++) {

                Component* newcomp = nullptr;
                for (auto c : newobj->s->comps) {
                    if (!strcmp(newobj->s->comps[comp_id]->name.c_str(),c->name.c_str())) {
                        
                        newcomp = c;
                        break;

                    }
                    newcomp_offset+= c->size;
                }
                if (!newcomp) { PLOGW << "couldntfind" << newobj->s->comps[comp_id]->name; continue; }
                
                int bkpmember_offset = bkpcomp_offset;
                
                for (int member_id = 0; member_id < bkpobj.s->comps[comp_id]->members.size(); member_id++) {

                    int newmember_offset = newcomp_offset;
                    Member* newmember = nullptr;
                    for (auto &m : newcomp->members) {
                        
                        if (!strcmp(newcomp->members[member_id].name.c_str(),m.name.c_str())) {
                            
                            newmember = &m;
                            break;
                            
                        }
                        newmember_offset+= m.size;
                    }

                    if (!newmember) { PLOGW << "couldntfind" << newcomp->members[member_id].name; continue; }

                    Member* oldmember = &bkpobj.s->comps[comp_id]->members[member_id];
                           
                    auto bkpoffset = bkpobj.offset+(bkpobj.s->size()*entry_id)+bkpmember_offset;
                    

                    PLOGD << oldmember->name<< bkpmember_offset << " - " << newmember_offset;


                    // if(newobj->buffer->data.size()<= newobj->data(entry_id)+newmember_offset)PLOGW << "ZUBIMALEKOUM";

                    
                    memcpy(
 
                        newobj->data(entry_id)+newmember_offset, // dst (new) // should not be bkpmember_offset but newmember_offset

                        &bkp.data[bkpoffset], // src (old)

                        bkpobj.s->comps[comp_id]->members[member_id].size // old size

                    );

                           
                    bkpmember_offset += bkpobj.s->comps[comp_id]->members[member_id].size;

                }

                bkpcomp_offset += bkpobj.s->comps[comp_id]->size;

                // gotta delete comp here

            }

        }

    }

    int obj_offset = 0;
    for (auto& o : objects) {

        o.offset = obj_offset;
        obj_offset += o.size();

    }
    
    PLOGD << "transpose";

}