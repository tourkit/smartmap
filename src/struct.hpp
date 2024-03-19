#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <set>

#include "member.hpp"
#include "data.hpp"
#include "log.hpp"



    struct Struct : Member {

        Struct(std::string name = "", uint32_t quantity = 1) : Member(name) { quantity_v = quantity; if (!name.length()) quantity_v = 0; }

        ~Struct() { for (auto m : members) if (m->typed()) {
            
                    auto to_delete = m;
                    members.erase(std::remove(members.begin(), members.end(), m), members.end());
                    delete to_delete;} }

        static inline std::set<Struct*> owned;

        template <typename... Args> 
        static Struct& create(Args&&... args) { return **owned.insert(new Struct(std::forward<Args>(args)...)).first; }

        static void clear() { for ( auto s : owned ) delete s;  }

        static bool destroy(std::string name) { 

            for (auto &s : owned) if (s->name() == name) { owned.erase(s); delete &s; return true; }

            return false;

        }

        virtual Struct& add(Struct& s) { 

            for (auto &c : pool) {
                if (c == &s) {
                    return add(&s); // why check if s is already in pool  ?
                }
            }

            PLOGW << " noadd" << s.name(); return *this;
            
        }  

        Struct& add(const char* name) { 

            for (auto &s : owned) {
                if (!strcmp(name,s->name().c_str())) {
                    return add(*s); 
                }
            }

            PLOGW << " noadd" << name; return *this;
            
        }  

        Struct& remove(Struct& s) { 

            for (auto &m : members) if (m == &s) return remove(&s);

            PLOGW << " noadd" << s.name(); return *this;

        };
    
        template <typename T> 
        Struct& add(std::string name = "") { return add(new Data<T>(name)); }
    
        Struct& range(float from, float to) {   
            
            auto a = members.back();
            if (typeid(*a).hash_code() == typeid(Data<float>).hash_code()) {
                ((Data<float>*)members.back())->range_from = from;
                ((Data<float>*)members.back())->range_to = to;
                PLOGD << " ----- is : float";
            }

            if (typeid(*a).hash_code() == typeid(Data<uint32_t>).hash_code()) {
                ((Data<uint32_t>*)members.back())->range_from = (uint32_t) from;
                ((Data<uint32_t>*)members.back())->range_to = (uint32_t) to;
                PLOGD << " ----- is : uint32_t";
            }

            return *this; 
        
        }

        uint32_t size() override { 
            
            if (members.size() == 1) { return members[0]->size(); } 
            
            uint32_t size_v = 0;
            
            for (auto m : members) size_v += m->footprint();

            return size_v; 

        }

        uint32_t footprint() override { 

            if (striding()) return nextFactor2(size_v,16);
            
            return size_v; 
        
        } 
        std::type_index type() override { if (typed()) { return members[0]->type(); } return typeid(Struct); }

        bool owns(Member& m) override { 

            for (auto &s : members) if (s == &m) return true;

            return false;
            
        }

        void each(std::function<void(Member&, int, int)> cb, std::function<void(Member&)> after_cb = nullptr) { each([cb](Member& m, int offset, int depth){ cb(m,offset,depth); }, 0, 0, after_cb); }

        void each(std::function<void(Member&, int)> cb, std::function<void(Member&)> after_cb = nullptr)  { each([cb](Member& m, int offset, int depth){ cb(m,offset); }, 0, 0, after_cb); }
        
        void each(std::function<void(Member&)> cb, std::function<void(Member&)> after_cb = nullptr)  { each([cb](Member& m, int offset, int depth){ cb(m); }, 0, 0, after_cb); }

        void update() override { 

            size_v = 0;

            for (auto &m : members) size_v += m->footprint_all();

            Member::update();

         }

        Member* copy(Member* x = nullptr) override { 
            
            if (!x) x = new Struct(name_v); 
            
            return Member::copy(x);  
            
        }

        virtual Struct& remove(Member*  m) {

            size_v -= members.back()->footprint_all();

            members.erase(std::remove(members.begin(),members.end(),m),members.end());

            update();

            return *this; 

        }

         void each(std::function<void(Member& m, int offset, int depth)> cb, int offset, int depth, std::function<void(Member&)> after_cb) override {

            for (int i = 0 ; i < quantity(); i++) {

                cb(*this, offset,depth);

                int size = 0;

                if (members.size() > 1 || members[0]->name().length() ||  striding()) {
                
                    for (auto &m :members) {
                        
                        m->each(cb, offset+size, depth+1, after_cb);

                        size+=m->footprint_all();

                    }

                    if (i!=quantity()-1) offset+=footprint();
                    
                }
                
                if (after_cb) after_cb(*this);

            }

        }        

    protected:
        virtual Struct& add(Member* s) {

            PLOGV << "add " << s->name() << " to " << name();

            members.push_back(s);

            size_v += members.back()->footprint_all();

            update();

            return *this;

        } 
    };
