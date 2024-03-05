#pragma once

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

        template <typename T>
        void set(T data) { memcpy(this->data(),(char*)&data, sizeof(T)); }

        int id;

        Entry *entry;

        int offset;

        Comp(int id, Entry* entry);

        char* data();

        Member operator[](const char* name);
        Member operator[](int id);


    };

    Comp operator[](int id);

    Comp operator[](const char* name);

    char* data();

};