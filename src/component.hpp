#pragma once

#include "pch.hpp"
#include <typeinfo>

struct Component {
   
    static inline std::vector<Component*> pool;

    struct Member {

        std::string name;
        size_t size;
        float range_from,range_to;  
        enum Type { UNDEFINED, F16, I8, I16, UI8, UI16 } type;

    };

    std::string name;
    size_t size;
    std::vector<Member> members;

    template <typename T>
    Component& member(std::string name = "") {

        float range_to = 1;

        Member::Type type = Member::Type::UNDEFINED;
        if (typeid(T) == typeid(float)) { 
            type = Member::Type::F16; 

        }else if (typeid(T) == typeid(uint8_t)) { 
            type = Member::Type::UI8; 
            range_to = 255;

        }else if (typeid(T) == typeid(uint16_t)) { 
            type = Member::Type::UI16; 

        }else if (typeid(T) == typeid(int8_t)) { 
            type = Member::Type::I8; 

        }else if (typeid(T) == typeid(int16_t)) { 
            type = Member::Type::I16; 
        }
        
        this->size += sizeof(T);
        
        members.push_back({name, sizeof(T), 0,range_to, type});

        return *this;

    }

    Component& range(float range_from, float range_to) {

        members.back().range_from = range_from;
        members.back().range_to = range_to;

        return *this;

    }

    static Component *id(const char* name) {

        for (int i = 0; i < pool.size(); i++) if (!strcmp(pool[i]->name.c_str(),name)) return pool[i];

        std::cout << "WARNING: Component \"" << name << "\" does not exist yet !" << std::endl;

        return &create(name);

    }

    static Component &create(const char* name) {

        pool.push_back(new Component{name,0});

        return *pool.back();

    }

    static void init() {

        pool.resize(0);

        create("RGB")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
        ;
        create("Opacity")
            .member<float>("alpha")
        ;

        create("Position")
            .member<float>("x").range(-1,1)
            .member<float>("y").range(-1,1)
        ;

        create("Size")
            .member<float>("x")
            .member<float>("y")
        ;
        create("Gobo")
            .member<float>("ID")
            .member<float>("FX1")
            .member<float>("FX2")
            .member<float>("FX3")
        ;

        create("Orientation")
            .member<float>("angle")
        ;

        create("Feedback")
            .member<float>("quantity")
        ;
        create("Strobe")
            .member<float>("speed")
        ;
        create("float")
            .member<float>("value")
        ;

    }

};

