#pragma once

#include "pch.hpp"
#include <typeinfo>

struct StringsBuffer {

  char* buffer = nullptr;
  const char** pointers = nullptr;

  ~StringsBuffer() { destroy(); }

  void destroy() {

      if (buffer) delete[] buffer;
      if (pointers) delete[] pointers;

  }

  void create(std::vector<std::string> strings) {

        destroy(); 

        size_t names_length = 0; 
        
        for (auto string:strings) names_length += string.size() + 1; 

        if (!names_length) names_length+=1;
        buffer = new char[names_length+1];

        memset(buffer,0,names_length+1);

        char* ptr = buffer;

        for (auto string:strings) { strcpy(ptr, string.c_str()); ptr += string.size() + 1; }

        pointers = new const char*[strings.size()];

        for (size_t i = 0; i < strings.size(); i++) { pointers[i] = ptr; ptr += strings[i].size() + 1; }

  }

};

struct Component {


    static inline StringsBuffer buffer_string;

    static void updateStringBufferList() {

            std::vector<std::string> names;
            for (auto &comp:Component::pool) { names.push_back(comp->name); }
            buffer_string.create(names);

    }
    static inline std::vector<Component*> pool;

    struct Member {

        std::string name;
        int size;
        // int offset;
        float range_from,range_to;  
        enum Type { UNDEFINED, F16, I8, I16, UI8, UI16, UI32 } type;

    };

    std::string name;
    int size =-1;
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

        }else if (typeid(T) == typeid(uint32_t)) { 
            type = Member::Type::UI32; 

        }else if (typeid(T) == typeid(int8_t)) { 
            type = Member::Type::I8; 

        }else if (typeid(T) == typeid(int16_t)) { 
            type = Member::Type::I16; 
        }

        this->size += sizeof(T);
        
        members.push_back({name, sizeof(T), 0,range_to, type});
        // members.push_back({name, sizeof(T), members.back().offset+members.back().size ,0,range_to, type});

        updateStringBufferList();
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

};

struct Components {


    static Components& getInstance() { static Components instance;  return instance; }
private:

    Components(const Components&) = delete;
    Components& operator=(const Components&) = delete;

    Components() {

        Component::pool.resize(0);

        Component::create("RGB")
            .member<float>("red")
            .member<float>("green")
            .member<float>("blue")
        ;
        Component::create("Opacity")
            .member<float>("alpha")
        ;

        Component::create("Position")
            .member<float>("x")//.range(-1,1)
            .member<float>("y")//.range(-1,1)
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
        Component::create("Vertex")
            .member<uint32_t>("id")
            ;
        Component::create("DimentionsUI32")
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

        auto &dmx = Component::create("DMX");
        for (size_t i = 0; i < 512; i++) dmx.member<uint8_t>("chan "+std::to_string(i));

    }
    

};

struct Object {

    std::string name;

    std::vector<Component*> components;

    int quantity,size = 0;
    
    // int get(int id, int comp, int member) { 
        
    //     return size*id; 
    
    // }

};
