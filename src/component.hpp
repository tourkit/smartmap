#pragma once

#include "glm/glm.hpp"
#include <typeinfo>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>
#include <cstring>


struct Member {

    std::string name;

    int size;
    // int offset;
    float range_from,range_to;  
    enum Type { UNDEFINED, F16, I8, I16, I32, UI8, UI16, UI32, VEC2, VEC3, VEC4, SAMPLER} type;
    float default_val = 0;

    const char* type_name() {

        switch (type) {
        
            case VEC2:
                return "vec2";
                break;
            case VEC3:
                return "vec3";
                break;
            case VEC4:
                return "vec4";
                break;
            case F16:
                return "float";
                break;
            case I16:
                return "int";
                break;
            case UI16:
                return "uint";
                break;
            case I32:
                return "int";
                break;
            
            case UI32:
                return "uint";
                break;
            case SAMPLER:
                return "Sampler2D";
                break;
            
            default:
                return "unknown";
                break;

        }

    }

};


struct Component  {
    
    std::string name;

    static inline std::vector<Component*> pool;

    int size;
    std::vector<Member> members;

    template <typename T>
    Component& member(std::string name = "", float range_from = 0, float range_to = 1) {


        Member::Type type = Member::Type::UNDEFINED;
        if (typeid(T) == typeid(float)) { 
            type = Member::Type::F16; 

        }else if (typeid(T) == typeid(uint8_t)) { 
            type = Member::Type::UI8; 
            range_to *= 255;

        }else if (typeid(T) == typeid(uint16_t)) { 
            type = Member::Type::UI16; 

        }else if (typeid(T) == typeid(uint32_t)) { 
            type = Member::Type::UI32; 

        }else if (typeid(T) == typeid(int8_t)) { 
            type = Member::Type::I8; 

        }else if (typeid(T) == typeid(int16_t)) { 
            type = Member::Type::I16; 

        }else if (typeid(T) == typeid(int32_t)) { 
            type = Member::Type::I32; 

        }else if (typeid(T) == typeid(glm::vec2)) { 
            type = Member::Type::VEC2; 

        }else if (typeid(T) == typeid(glm::vec3)) { 
            type = Member::Type::VEC3; 

        }else if (typeid(T) == typeid(glm::vec4)) { 
            type = Member::Type::VEC4; 

        }

        this->size += sizeof(T);
        
        members.push_back({name, sizeof(T), range_from,range_to, type});
        // members.push_back({name, sizeof(T), members.back().offset+members.back().size ,0,range_to, type});

        return *this;

    }

    Component& range(float range_from, float range_to, float default_val=0) {

        members.back().range_from = range_from;
        members.back().range_to = range_to;
        members.back().default_val = default_val;

        return *this;

    }

    static Component *exist(const char* name) {

        for (int i = 0; i < pool.size(); i++) if (!strcmp(pool[i]->name.c_str(),name)) return pool[i];

        return nullptr;

    }
    static Component *id(const char* name) {

        auto exist_v = exist(name);
        
        if (exist_v) return exist_v;

        return &create(name);

    }

    Component(std::string name, int size = -1) : name(name), size(size) {


    }

    static Component &create(const char* name) {

        pool.push_back(new Component{name,0});

        return *pool.back();

    }


    void each(std::function<void(Member &m)> cb) {

        for (auto &m:members) {
                    
            uint8_t q = 1;
            if (m.type == Member::Type::VEC2) q =2;
            if (m.type == Member::Type::VEC3) q =3;
            if (m.type == Member::Type::VEC4) q =4;
            for (int i = 0; i < q; i++) cb(m);

        }

    }

};

struct Components {


    static Components& getInstance() { static Components instance;  return instance; }
private:

    Components(const Components&) = delete;
    Components& operator=(const Components&) = delete;

    Components() ;
    

};
