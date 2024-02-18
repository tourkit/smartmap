#pragma once


#include "log.hpp"
#include "glm/glm.hpp"
#include <typeinfo>
#include <vector>
#include <string>
#include <cstdint>

struct Component  {
    
    std::string name;

    static inline std::vector<Component*> pool;

    struct Member {

        std::string name;

        int size;
        // int offset;
        float range_from,range_to;  
        enum Type { UNDEFINED, F16, I8, I16, UI8, UI16, UI32, VEC2, VEC3, VEC4 } type;
        float default_val = 0;

    };

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

};

struct Components {


    static Components& getInstance() { static Components instance;  return instance; }
private:

    Components(const Components&) = delete;
    Components& operator=(const Components&) = delete;

    Components() {

        Component::pool.resize(0);

        Component::create("RGBA")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
            .member<float>("alpha")
        ;
        Component::create("RGB")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
        ;
        Component::create("Opacity")
            .member<float>("alpha")
        ;

        Component::create("Position")
            .member<float>("x").range(-1,1)
            .member<float>("y").range(-1,1)
        ;
        Component::create("Position3D")
            .member<float>("x")//.range(-1,1)
            .member<float>("y")//.range(-1,1)
            .member<float>("z")//.range(-1,1)
        ;
        Component::create("U32Vec3")
            .member<uint32_t>("x")//.range(-1,1)
            .member<uint32_t>("y")//.range(-1,1)
            .member<uint32_t>("z")//.range(-1,1)
        ;
        Component::create("UV")
            .member<float>("x")
            .member<float>("y")
        ;

        Component::create("Size")
            .member<float>("x")
            .member<float>("y")
        ;
        Component::create("Gobo")
            .member<float>("ID")
            .member<float>("FX1")
            .member<float>("FX2")
            .member<float>("FX3")
        ;

        Component::create("Orientation")
            .member<float>("angle").range(-3.14159265359,3.14159265359)
        ;

        Component::create("Feedback")
            .member<float>("quantity")
        ;
        Component::create("Strobe")
            .member<float>("speed")
        ;
        Component::create("float")
            .member<float>("value")
        ;
        Component::create("char")
            .member<char>("value")
        ;
        Component::create("2chars")
            .member<uint8_t>("char1")
            .member<uint8_t>("char2")
        ;
        Component::create("Vec2")
            .member<float>("x")
            .member<float>("y")
        ;
        Component::create("Ratio")
            .member<float>("value")
        ;
        Component::create("int")
            .member<uint32_t>("value")
        ;
        Component::create("Vertex")
            .member<uint32_t>("id")
        ;
        Component::create("DimentionsUI32")
            .member<uint32_t>("width")
            .member<uint32_t>("height")
        ;
        Component::create("Resolution")
            .member<uint32_t>("width")
            .member<uint32_t>("height")
        ;
        Component::create("Dimentions")
            .member<float>("width")
            .member<float>("height")
        ;
        Component::create("ID")
            .member<uint32_t>("value")
        ;
        Component::create("Offset")
            .member<uint32_t>("value")
        ;

        auto &dmx = Component::create("DMX");
        for (size_t i = 0; i < 512; i++) dmx.member<uint8_t>("chan "+std::to_string(i));

    }
    

};
