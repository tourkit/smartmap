#pragma once

#include "object.hpp"
#include "struct.hpp"
#include "component.hpp"

#include <cstring>

struct Object;

struct Entry {

    Object* obj;

    int id;

    struct Comp {

        struct Member{

            char* data;

            template <typename T>
            void set(T data) { memcpy(this->data,(char*)&data,sizeof(data)); }

            template <typename T>
            T& get() { return *((T*)data); }
        
        };

        int id;

        Entry *entry;

        int offset;

        Comp(int id, Entry* entry) : id(id), entry(entry) { 

            offset = 0;
            int curr = 0;
            for (auto c : entry->obj->s->comps) {
                if (curr++ >= id) break;
                offset += c->size;
            }

        }

        char* data() { return entry->data()+offset; }

        Member operator[](const char* name) { 
        
            int id = 0;

            for (auto m : entry->obj->s->comps[id]->members) { if (!(strcmp(m.name.c_str(),name))) { break;} id++; }

            return (*this)[id];

        }
        Member operator[](int id) { 
            
            int member_offset = 0;
            int current = 0;
            for (auto m : entry->obj->s->comps[this->id]->members) {
                if (current++ == id) break;
                member_offset += m.size;
            }

            return Member{data()+member_offset};
            
        }


    };

    Comp operator[](int id) { 
        
        
        return Comp{id,this}; 
        
    }

    Comp operator[](const char* name) { 
        
        int id = 0;

        for (auto c : obj->s->comps) { if (!(strcmp(c->name.c_str(),name))) { break;} id++; }

        return (*this)[id];
    
    }

    char* data() { return obj->data(id); }

};