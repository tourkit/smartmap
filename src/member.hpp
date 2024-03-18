#pragma once

#include "glm/glm.hpp"
#include "log.hpp"
#include <cstdint>
#include <unordered_set>
#include <typeindex>

static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }

    struct Member {
    
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

        static inline std::unordered_set<Member*> pool;

        Member(std::string name_v = "") { name(name_v); 
        
        PLOGV << name(); 
        
        pool.insert(this);}

        virtual ~Member() { 
            
            PLOGV << "~" << name(); 
            
            pool.erase(this); 
        
            for (auto m : pool) {

            m->each([this](Member& m) { 
                
                if (m.owns(*this)) {
                    
                    m.members.erase(std::remove(m.members.begin(), m.members.end(), this), m.members.end()); 
                    
                    }
                

                });

        }

        }

        uint32_t quantity = 1;

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
        
        uint32_t eq(int i) { return i*footprint(); } 

        virtual uint32_t footprint_all() { return footprint() * quantity; }

        virtual std::type_index type() { return typeid(*this); }
        
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

        virtual Member* copy(Member* x = nullptr) { 

            if(!x) x = new Member(name_v); 

            x->striding(is_striding);

            x->quantity = quantity;    

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
                str += !m.typed() ? "struct" : m.type().name();
                str += " " + m.name();
                str += "    " +  std::to_string(offset);
                str += " (" +std::to_string(m.footprint())+")";

                PLOGD << str;

            }, 0, 0, [&](Member& m){ if (m.is_striding && m.stride()) PLOGD << tab <<"stride    " << m.stride() << " (" << m.name() << ")"; });

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

    protected:
        std::string name_v;
        uint32_t size_v = 0;
        bool is_striding = false;
    private:
    


    };
