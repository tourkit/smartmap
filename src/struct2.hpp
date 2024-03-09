#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <set>
#include "log.hpp"



static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }

namespace TEST {

    struct AnyMember {

        static inline std::unordered_set<AnyMember*> pool;

        AnyMember(std::string name_v = "") { name(name_v); pool.insert(this); }

        ~AnyMember() { PLOGD << "~" << name(); pool.erase(this); }

        uint32_t quantity = 1; // ! \\ ONLY FOR Array !

        virtual bool owns(AnyMember& m) { return false; }

        virtual void update() { for (auto a : pool) if (a->owns(*this)) a->update(); }

        virtual uint32_t size() { return 0; }

        virtual uint32_t footprint() { 

            if (is_striding) return nextFactor2(size_v,16);
            
            return size_v; 
        
        } 

        virtual uint32_t footprint_all() { return footprint() * quantity; }

        virtual std::string type() { return "uknw"; }
        
        void name(std::string name_v) { this->name_v = name_v; }

        virtual std::string name() { return name_v; }

        AnyMember* any() { return this; }

        virtual void each(std::function<void(AnyMember& m, int offset, int depth)> cb, int offset, int depth, std::function<void(AnyMember&)> after_cb = nullptr) { cb(*this, offset,depth); }

        virtual void print(int tab = 0) {

            std::string str;
            for (int i = 0; i < tab; i++) str += "  ";
            str += type() + " " + name();
            if (quantity>1) str += "[" + std::to_string(quantity) + "] - " + std::to_string(footprint()) + "B * " + std::to_string(quantity) + " = " + std::to_string(footprint_all()) + "B";
            else str += " - " + std::to_string(footprint()) + "B";
            PLOGD << str ;

        }

        void* range_from_ptr = nullptr;
        void* range_to_ptr = nullptr;
        void* default_val_ptr = nullptr;

        uint32_t stride() { return (footprint()-size_v); }

        void striding(bool is_striding){ this->is_striding = is_striding; update(); }

        bool striding() { return is_striding; }

        std::vector<AnyMember*> members;

    protected:

        bool is_striding = false;
    
        uint32_t size_v = 0;

    private:
    
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

        std::string type() override { return std::string(typeid(T).name()); }

        uint32_t size() override { return sizeof(T); }

        uint32_t footprint() override { return size(); }

    };


    struct Struct : AnyMember {

        Struct(std::string name = "", uint32_t quantity = 1) : AnyMember(name) { this->quantity = quantity; }

        static inline std::set<Struct*> owned;

        template <typename... Args> 
        static Struct& create(Args&&... args) { 
            
            return **owned.insert(new Struct(std::forward<Args>(args)...)).first;
            
        }

        static bool destroy(std::string name) { 

            for (auto &s : owned) if (s->name() == name) { owned.erase(s); delete &s; return true; }

            return false;

        }


        Struct copy() {

            Struct s = * this;
            // for (auto &m : members) // make copry

            return s;

        }

        Struct&  resize(int quantity) {

            this->quantity = quantity;
            
            update();

            return *this;
        
        }

        Struct& add(Struct& s) { 

            for (auto &c : pool) if (c == &s) return addPtr(&s);

            PLOGW << " noadd" << s.name();

            return *this;
            
        }  

        Struct& remove(Struct& s) { 

            for (auto &m : members) if (m == &s) return removePtr(&s);

            PLOGW << " noadd" << s.name();

            return *this;

        };
    
        template <typename T> 
        Struct& add(std::string name = "") { return addPtr(new Data<T>(name)); }
    
        Struct& range(float from, float to) { 
            
            auto a = members.back();
            if (typeid(*a).hash_code() == typeid(Data<float>).hash_code()) {
                ((Data<float>*)members.back())->range_from = from;
                ((Data<float>*)members.back())->range_to = to;
            }

            if (typeid(*a).hash_code() == typeid(Data<uint32_t>).hash_code()) {
                ((Data<uint32_t>*)members.back())->range_from = (uint32_t) from;
                ((Data<uint32_t>*)members.back())->range_to = (uint32_t) to;
            }

            return *this; 
        
        }

        uint32_t size() override { 
            
            if (members.size() == 1) { return members[0]->size(); } 
            
            uint32_t size_v = 0;
            
            for (auto &m : members) size_v += m->size();

            return size_v; 

        }

        std::string type() override { if (members.size() == 1) { return members[0]->type(); } return "Struct"; }

        bool owns(AnyMember& m) override { 

            for (auto &s : members) if (s == &m) return true;

            return false;
        }

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

        void each(std::function<void(AnyMember&, int, int)> cb, std::function<void(AnyMember&)> after_cb = nullptr) { each([cb](AnyMember& m, int offset, int depth){ cb(m,offset,depth); }, 0, 0, after_cb); }

        void each(std::function<void(AnyMember&, int)> cb, std::function<void(AnyMember&)> after_cb = nullptr)  { each([cb](AnyMember& m, int offset, int depth){ cb(m,offset); }, 0, 0, after_cb); }
        
        void each(std::function<void(AnyMember&)> cb, std::function<void(AnyMember&)> after_cb = nullptr)  { each([cb](AnyMember& m, int offset, int depth){ cb(m); }, 0, 0, after_cb); }

        void print(int tab = 0) override {

            AnyMember::print(tab);

            int offset = 0;

            if (members.size() > 1 || members[0]->name().length() ||  striding()) {
            
                for (auto &m :members) {
                    
                    m->print(tab+1);

                    offset+=m->footprint_all();

                }

            }

            if (is_striding) {

                int stride = nextFactor2(offset,16)-offset;
                if (!stride) return;
                std::string str;
                for (int i = 0; i < tab+1; i++) str+="  ";
                str += "  char stride[" + std::to_string(stride) + "] - 1B";
                PLOGD << str;

            } 
        }

        void update() override { 

            size_v = 0;

            for (auto &m : members) size_v += m->footprint_all();

            AnyMember::update();

         }


    protected:
        
        virtual Struct& addPtr(AnyMember* s) {

            members.push_back(s);

            size_v += members.back()->footprint_all();

            update();

            return *this;

        } 

        virtual Struct& removePtr(AnyMember*  m) {


            size_v -= members.back()->footprint_all();
            members.erase(std::remove(members.begin(),members.end(),m),members.end());

            update();

            return *this; 

        }

    };




};