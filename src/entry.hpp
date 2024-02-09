#pragma once

#include "object.hpp"
#include "struct.hpp"
#include "component.hpp"

#include <cstring>

struct Object;

struct Entry {

    Object* obj;
    int id;

    const char* name();

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

        Comp(int id, Entry* entry) : id(id), entry(entry) {}

        int offset = 0;

        Comp(int id);

        
        Member operator[](int id) { 

            int current = 0;
            
            int offset = 0;

            for (auto m : entry->obj->s->comps[this->id]->members) {

                if (current++ == id) break;

                offset += m.size;


            }

            return Member{entry->obj->data(entry->id)+this->offset+offset};
            
        }

        
        Member operator[](const char* name) { 
        
            int id = 0;

            for (auto m : entry->obj->s->comps[id]->members) { if (!(strcmp(m.name.c_str(),name))) { break;} id++; }

            return (*this)[id];

        }



    };

    Comp operator[](int id) { return Comp(id,this); }

    Comp operator[](const char* name) { 
        
        int id = 0;

        for (auto c : obj->s->comps) { if (!(strcmp(c->name.c_str(),name))) { break;} id++; }

        return (*this)[id];
    
    }

};