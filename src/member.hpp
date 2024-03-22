#pragma once

#include "glm/glm.hpp"
#include "log.hpp"
#include <cstdint>
#include <unordered_set>
#include <typeindex>

static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }

struct Member {

    static inline std::unordered_set<Member*> pool; // has a ptr to all axisting Members;

    Member(std::string name_v = "") { 
        
        name(name_v); 
    
        PLOGV << "#" << name(); 
        
        pool.insert(this);
        
    }

    virtual ~Member();

    uint32_t quantity_v = 1;

    virtual bool owns(Member& m) { return false; }

    virtual void update() { 
        PLOGV<<name(); 
        for (auto a : pool) {
            if (a->owns(*this)) {
                a->update();
            } 
        }
    }

    virtual uint32_t size() { return 0; }

    virtual uint32_t footprint() { return 0; } 

    void quantity(uint32_t quantity_v) { this->quantity_v = quantity_v; update(); }

    uint32_t quantity() { return quantity_v; }
    
    uint32_t eq(int i) { return i*footprint(); } 

    virtual uint32_t footprint_all() { return footprint() * quantity_v; }

    virtual std::type_index type() { return typeid(*this); }

    const char* type_name() {
        
        if (type() == typeid(glm::vec2)) return "vec2";
            
        if (type() == typeid(glm::vec3)) return "vec3";
            
        if (type() == typeid(glm::vec4)) return "vec4";
            
        if (type() == typeid(float)) return "float";
            
        if (type() == typeid(int32_t)) return "int";
            
        if (type() == typeid(uint32_t)) return "uint";
        
            // return "Sampler2D";
            
        return "unknown";

    }

    uint8_t count() { 
        
        // if (type() == typeid(Struct)) { int x = 0; each([](Member* m){ x += m.count()}; ); return x; }

        if (type() == typeid(glm::vec2)) return 2;
            
        if (type() == typeid(glm::vec3)) return 3;
            
        if (type() == typeid(glm::vec4)) return 4;
        
        return 1; 
        
    }

    void name(std::string name_v) { this->name_v = name_v; }

    virtual std::string name() { return name_v; }

    Member* any() { return this; }

    virtual void each(std::function<void(Member& m, int offset, int depth)> cb, int offset, int depth, std::function<void(Member&)> after_cb = nullptr) { cb(*this, offset,depth); }

    void each(std::function<void(Member&)> cb)  { each([cb](Member& m, int offset, int depth){ cb(m); }, 0, 0, nullptr); }

    void* range_from_ptr = nullptr;
    void* range_to_ptr = nullptr;
    void* default_val_ptr = nullptr;

    uint32_t stride() { return (footprint()-size()); }

    void striding(bool is_striding){ this->is_striding = is_striding; update(); }

    bool striding() { return is_striding; }

    virtual Member* copy(Member* x = nullptr) { 

        if(!x) x = new Member(name_v); 

        x->striding(striding());

        x->quantity_v = quantity_v;    

        x->members = members;

        for (auto &m : x->members) m = m->copy();
        
        x->size_v = size_v;
        return x; 
        
    }
    
    virtual bool typed() { return false; if (members.size() == 1 && members[0]->name_v.length()) return true; return false; }

    std::vector<Member*> members;
    
    void print() {

        std::string tab;

        each([&](Member& m, int offset, int depth){ 
            
            tab = ""; for (int i = 0 ; i < depth; i++) tab+= "  ";

            std::string str; 
            str += tab;
            str += !m.typed() ? "struct" : m.type_name();
            str += " " + m.name();
            str += "    " +  std::to_string(offset);
            str += " (" +std::to_string(m.footprint())+")";

            PLOGD << str;

        }, 0, 0, [&](Member& m){ if (m.striding() && m.stride()) PLOGD << tab <<"stride    " << m.stride() << " (" << m.stride() << ")"; });

    }

    void hard_delete() { 

        for (auto &m : members) {
            
            m->hard_delete();

            if (!m->typed()) {

                auto to_delete =  m;

                delete m;
                
                members.erase(std::remove(members.begin(), members.end(), to_delete), members.end());
                
            }

        }

    }

    uint32_t size_v = 0;
protected:
    std::string name_v;

    Member& add(Member* s);
    
    Member& remove(Member* s) {

        if (std::find(members.begin(), members.end(), s) != members.end()) {

            PLOGV << "remove " << s->name() << " from " << name();
            
            members.erase(std::remove(members.begin(), members.end(), s), members.end()); 

        }

        return *this;

    } 


private:

    bool is_striding = false;


};
