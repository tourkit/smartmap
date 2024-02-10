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

        char* data() { 

            int comp_offset = 0;
            int curr = 0;
            for (auto c : entry->obj->s->comps) {
                if (curr++ >= id) break;
                comp_offset += c->size;
            }

            return entry->data()+comp_offset; 
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

    char* data() { return obj->data(id); }

};