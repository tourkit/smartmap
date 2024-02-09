#pragma once

#include "object.hpp"
#include "struct.hpp"
#include "component.hpp"

// #include <sizeof>

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
        
        Object *obj;

        Comp(int id, Object* obj) : id(id), obj(obj) {}

        int offset = 0;

        Comp(int id);

        
        Member operator[](int id) { 

            int current = 0;
            
            int offset = 0;

            for (auto m : obj->s->comps[this->id]->members) {

                if (current++ == id) break;

                offset += m.size;


            }

            return Member{obj->data(this->id)+this->offset+offset};
            
        }

        
        Member operator[](std::string name) { 
        
            int id = 0;

            for (auto m : obj->s->comps[id]->members) { if (m.name == name) { break;} id++; }

            return (*this)[id];

        }



    };

    Comp operator[](int id) { return Comp(id,obj); }

    Comp operator[](std::string name) { 
        
        int id = 0;

        for (auto c : obj->s->comps) { if (c->name == name) { break;} id++; }

        return (*this)[id];
    
    }

};