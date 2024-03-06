#pragma once

#include "struct.hpp"
#include "component.hpp"

#include <cstring>

struct Object;

struct Instance {

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

        Instance *instance;

        int offset;

        Comp(int id, Instance* instance);

        char* data();

        Member operator[](const char* name);
        Member operator[](int id);


    };

    Comp operator[](int id);

    Comp operator[](const char* name);

    char* data();

};