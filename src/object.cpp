#include "object.hpp"

#include <memory>
#include <cstring>

#include "struct.hpp"
#include "buffer.hpp"
#include "entry.hpp"

size_t Object::size() { return eq(reserved); }

char *Object::data(size_t id) { return &buffer->data[offset + eq(id)]; }

int Object::eq(size_t id) { return ((s->size()+s->stride()) * id); }

Entry &Object::push() { 

    std::vector<char> data;
    data.resize(s->size());
    memset(&data[0],0,data.size());

    return push(&data[0]);

}

void Object::clear() {

    

}


void Object::update(Buffer bkp) {

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
                           
                    auto offset = o.offset+(o.s->size()*entry_id)+member_offset;

                    auto x = &backup_data[offset];

                    memcpy((*buffer)[obj_id]->data(entry_id)+member_offset,x,o.s->comps[comp_id]->members[member_id].size);
                           
                    member_offset += o.s->comps[comp_id]->members[member_id].size;

                }

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

void Object::addComp(std::string component){

    // need stride ctrl, ds obj.update ou buffer.update ?

    auto s = *this->s;
    auto s_ptr = this->s;
    this->s = &s;
    Buffer bkp = *buffer;
    this->s = s_ptr;

    this->s->addComp(component);

    update(bkp);

    // niquons l'opti :) | whats teh pb ?
    for (auto e : entrys){

        auto c = (*e)[component.c_str()];

        int i = 0;
        for (auto member : Component::id(component.c_str())->members) {

            auto w = &c[i++].get<float>();        
            (*w) = member.default_val;
            if (member.type == Member::Type::VEC2) { *(w+1) = member.default_val; }
            if (member.type == Member::Type::VEC3) { *(w+1) = member.default_val; *(w+2) = member.default_val; }
            if (member.type == Member::Type::VEC4) { *(w+1) = member.default_val; *(w+2) = member.default_val; *(w+3) = member.default_val; }
            
        }

    }

    buffer->update();

}


Entry &Object::push(void* data) { 

    Buffer bkp = *buffer;

    reserved+=1;

    update(bkp);

    int id = entrys.size();

    memcpy(this->data()+eq(id),data,s->size()); 

    entrys.push_back(new Entry{this,id});

    // shouldnt buffer->update() here ? if not find where does it happn now and if is logic ?
    
    return *entrys.back();

}   