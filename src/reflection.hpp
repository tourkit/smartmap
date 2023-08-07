#pragma once

#include "pch.hpp"
#include "components2.hpp"
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

            Member::Type type = Member::Type::UNDEFINED;
            if (typeid(T) == typeid(float)) type = Member::Type::F16;
            else if (typeid(T) == typeid(uint8_t)) type = Member::Type::UI8;
            else if (typeid(T) == typeid(uint16_t)) type = Member::Type::UI16;
            else if (typeid(T) == typeid(int8_t)) type = Member::Type::I8;
            else if (typeid(T) == typeid(int16_t)) type = Member::Type::I16;

            members.push_back({name, sizeof(T), 0,1, type});

            return *this;

        }

        Component& range(float range_from, float range_to) {

            members.back().range_from = range_from;
            members.back().range_to = range_to;

            return *this;

        }

    };

    static inline std::map<size_t, Component> components2;
    static inline std::map<const char*, Component> components;

    std::vector<Component> definition;

    template <typename T>
    static Component &component(std::string name) {

        size_t uid = typeid(T).hash_code();

        components2[uid] = {name,sizeof(T)};

        return components2[uid];

    }

    template <typename T>
    static Component &component() { 

        size_t uid = typeid(T).hash_code();
        
        if (components2.find(uid) != components2.end()) components2[uid] = {typeid(T).name(),sizeof(T)};

        return components2[uid];
    
    }

    static Component &component(const char* name) {

        if (components.find(name) != components.end()) components[name] = {sizeof(T)};

        return components[name];

    }

};