#pragma once

#include "pch.hpp"
#include <typeinfo>


struct Reflection {

    struct Member {

        std::string name;
        size_t size;
        float range_from,range_to;  
        enum Type { UNDEFINED, F16, I8, I16, UI8, UI16 } type;

    };

    struct Component {

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

    };

    static inline std::vector<Component> components;

    static Component &component(const char* name) {

        for (int i = 0; i < components.size(); i++) if (!strcmp(components[i].name.c_str(),name)) return components[i];

        components.push_back({name,0});

        return components.back();

    }

    std::vector<Component> structure;

};