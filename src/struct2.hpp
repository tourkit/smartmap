#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <set>
#include <typeindex>
#include "log.hpp"

static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }

namespace TEST {

    struct AnyMember {

        static inline std::unordered_set<AnyMember*> pool;

        AnyMember(std::string name_v = "") { name(name_v); pool.insert(this); }

        ~AnyMember() { pool.erase(this); }

        uint32_t quantity = 1; // ! \\ ONLY FOR Array !

        virtual bool owns(AnyMember& m) { return false; }

        virtual void update() { for (auto a : pool) if (a->owns(*this)) a->update(); }

        virtual uint32_t size() { return 0; }

        virtual uint32_t footprint() { return 0; } 

        virtual uint32_t footprint_all() { return footprint() * quantity; }

        virtual std::type_index type() { return typeid(*this); }
        
        void name(std::string name_v) { this->name_v = name_v; }

        virtual std::string name() { return name_v; }

        AnyMember* any() { return this; }

        virtual void each(std::function<void(AnyMember& m, int offset, int depth)> cb, int offset, int depth, std::function<void(AnyMember&)> after_cb = nullptr) { cb(*this, offset,depth); }

        void* range_from_ptr = nullptr;
        void* range_to_ptr = nullptr;
        void* default_val_ptr = nullptr;

        uint32_t stride() { 
            auto f = footprint();
            auto n = name();
            auto s = size();
             return (f-s); 
            
            }

        void striding(bool is_striding){ this->is_striding = is_striding; update(); }

        bool striding() { return is_striding; }

        virtual AnyMember* copy(AnyMember* x = nullptr) { 

            if(!x) x = new AnyMember(); 
            
            x->name(name()+"_bkp");

            x->striding(striding());

            x->members = members;
            
            return x; 
            
        }
        
        virtual bool typed() { return false; }

        std::vector<AnyMember*> members;

        void print() {

            std::string tab;

            each([&](AnyMember& m, int offset, int depth){ 
                
                tab = ""; for (int i = 0 ; i < depth; i++) tab+= "  ";

                std::string str; 
                str += tab;
                str += !m.typed() ? "struct" : m.type().name();
                str += " " + m.name();
                str += "    " +  std::to_string(offset);
                str += " (" +std::to_string(m.footprint())+")";

                PLOGD << str;

            }, 0, 0, [&](AnyMember& m){ if (m.striding()) PLOGD << tab <<"stride    " << m.stride() << " (" << m.name() << ")"; });

        }

    private:
    
        bool is_striding = false;
        std::string name_v;

    };

    struct AnyData : AnyMember {         
        
        template <typename... Args> 
        AnyData(Args&&... args) : AnyMember(std::forward<Args>(args)...) { } 
        
    };
    
    template <typename T>
    struct Data : AnyData {

        T range_from;
        T range_to;
        T default_val;

        template <typename... Args> 
        Data(Args&&... args) : AnyData(std::forward<Args>(args)...) {

            range_from_ptr = &range_from;
            range_to_ptr = &range_to;
            default_val_ptr = &default_val;

        }

        std::type_index type() override { return typeid(T); }

        uint32_t size() override { return sizeof(T); }

        uint32_t footprint() override { return size(); }
        
        bool typed() override { return true; }

        AnyMember* copy(AnyMember* x = nullptr) override { 
            
            if (!x) x = new Data<T>(name()); 
            
            AnyMember::copy(x);

            memcpy(x->range_from_ptr,range_from_ptr,sizeof(T));
            memcpy(x->range_to_ptr,range_to_ptr,sizeof(T));
            memcpy(x->default_val_ptr,default_val_ptr,sizeof(T));

            return x; 
            
        }

    };

    struct Struct : AnyMember {

        Struct(std::string name = "", uint32_t quantity = 1) : AnyMember(name) { this->quantity = quantity; }

        ~Struct() { for (auto m : members) if (m->typed()) delete m; }

        static inline std::set<Struct*> owned;

        template <typename... Args> 
        static Struct& create(Args&&... args) { return **owned.insert(new Struct(std::forward<Args>(args)...)).first; }

        static bool destroy(std::string name) { 

            for (auto &s : owned) if (s->name() == name) { owned.erase(s); delete &s; return true; }

            return false;

        }

        Struct&  resize(int quantity) {

            this->quantity = quantity;
            
            update();

            return *this;
        
        }

        Struct& add(Struct& s) { 

            for (auto &c : pool) if (c == &s) return add(&s);

            PLOGW << " noadd" << s.name(); return *this;
            
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
            
            for (auto m : members) {
                auto s = m->footprint();
                size_v += s;
                }

            return size_v; 

        }
        uint32_t footprint() override { 

            if (striding()) return nextFactor2(size_v,16);
            
            return size_v; 
        
        } 
        std::type_index type() override { if (members.size() == 1) { return members[0]->type(); } return typeid(Struct); }

        bool owns(AnyMember& m) override { 

            for (auto &s : members) if (s == &m) return true;

            return false;
            
        }



        void each(std::function<void(AnyMember&, int, int)> cb, std::function<void(AnyMember&)> after_cb = nullptr) { each([cb](AnyMember& m, int offset, int depth){ cb(m,offset,depth); }, 0, 0, after_cb); }

        void each(std::function<void(AnyMember&, int)> cb, std::function<void(AnyMember&)> after_cb = nullptr)  { each([cb](AnyMember& m, int offset, int depth){ cb(m,offset); }, 0, 0, after_cb); }
        
        void each(std::function<void(AnyMember&)> cb, std::function<void(AnyMember&)> after_cb = nullptr)  { each([cb](AnyMember& m, int offset, int depth){ cb(m); }, 0, 0, after_cb); }

        void update() override { 

            size_v = 0;

            for (auto &m : members) size_v += m->footprint_all();

            AnyMember::update();

         }

        AnyMember* copy(AnyMember* x = nullptr) override { 
            
            if (!x) x = new Struct(); 
            
            AnyMember::copy(x);  

            x->quantity = quantity;    

            for (auto m : members) m = m->copy();

            return x; 
            
        }


    protected:
         void each(std::function<void(AnyMember& m, int offset, int depth)> cb, int offset, int depth, std::function<void(AnyMember&)> after_cb) override {

            for (int i = 0 ; i < quantity; i++) {

                cb(*this, offset,depth);

                int size = 0;

                if (members.size() > 1 || members[0]->name().length() ||  striding()) {
                
                    for (auto &m :members) {
                        
                        m->each(cb, offset+size, depth+1, after_cb);

                        size+=m->footprint_all();

                    }

                    if (i!=quantity-1) offset+=footprint();
                    
                }
                
                if (after_cb) after_cb(*this);

            }

        }        
        virtual Struct& add(AnyMember* s) {

            members.push_back(s);

            size_v += members.back()->footprint_all();

            update();

            return *this;

        } 

        virtual Struct& remove(AnyMember*  m) {

            size_v -= members.back()->footprint_all();

            members.erase(std::remove(members.begin(),members.end(),m),members.end());

            update();

            return *this; 

        }
private :

    
        uint32_t size_v = 0;
    };




};