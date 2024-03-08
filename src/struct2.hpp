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

        static inline std::unordered_set<std::shared_ptr<AnyMember>> pool;

        AnyMember(std::string name_v = "") { name(name_v); pool.insert(std::shared_ptr<AnyMember>(this)); }

        ~AnyMember() { pool.erase(std::shared_ptr<AnyMember>(this)); }

        uint32_t quantity = 1; // ! \\ ONLY FOR Array !

        virtual bool owns(AnyMember& m) { return false; }

        virtual void update() { 

            for (auto a : pool) if (a.get()->owns(*this)) a.get()->update();

        }

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

        virtual void each(std::function<void(AnyMember& m, int offset)> cb, int offset = 0) { cb(*this, offset); }

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

        uint32_t stride() { return (footprint_all()-size_v); }

        void striding(bool is_striding){ this->is_striding = is_striding; update(); }

        bool striding() { return is_striding; }

        std::vector<std::shared_ptr<AnyMember>> members;

    protected:

        bool is_striding = false;
    
        uint32_t size_v = 0;

    private:
    
        std::string name_v;

    };


    template <typename T>
    struct Member : AnyMember {

        T range_from;
        T range_to;
        T default_val;

        Member(std::string name = "")  : AnyMember(name) {

            range_from_ptr = &range_from;
            range_to_ptr = &range_to;
            default_val_ptr = &default_val;

        }

        std::string type() override { return std::string(typeid(T).name()); }

        uint32_t size() override { return sizeof(T); }

        uint32_t footprint() override { return size(); }


    };

    struct Struct;
    // struct Instance : AnyMember {

    //     Struct* s;

    //     std::string name() override { if (quantity > 1) { return AnyMember::name()+"_"+std::to_string(quantity); } return AnyMember::name(); }

    // };

    struct Struct : AnyMember {

        Struct(std::string name = "", uint32_t quantity = 1) : AnyMember(name) { this->quantity = quantity; }

        static inline std::unordered_set<std::shared_ptr<Struct>> pool;

        template <typename... Args> 
        static Struct& create(Args&&... args) { 
            
            auto x = pool.insert(std::make_shared<Struct>(std::forward<Args>(args)...)); 
        
            return *x.first->get(); 
            
        }

        static bool destroy(std::string name) { 

            for (auto &s : pool) if (s->name() == name) { pool.erase(s); return true; }

            return false;

        }

        static Struct& find(std::string name) { 

            for (auto &s : pool) if (s->name() == name) return *s;
            
            return create(name);

         }

        Struct copy() {

            Struct s = * this;
            // for (auto &m : members) // make copry

            return s;

        }


        Struct& add(Struct& s) { return addPtr(std::shared_ptr<Struct>(&s)); }  

        Struct& remove(Struct& s) { ; PLOGD <<"OOO"; return removePtr(std::make_shared<Struct>(s)); }  
    
        template <typename T> 
        Struct& add(std::string name = "") { return addPtr(std::make_shared<Member<T>>(name)); }
    
        Struct& range(float from, float to) { 
            
            auto a = members.back().get();
            if (typeid(*a).hash_code() == typeid(Member<float>).hash_code()) {
                ((Member<float>*)members.back().get())->range_from = from;
                ((Member<float>*)members.back().get())->range_to = to;
            }
            return *this; 
        
        }
    
        Struct& range(uint32_t from, uint32_t to) { 
            
            auto a = members.back().get();
            if (typeid(*a).hash_code() == typeid(Member<uint32_t>).hash_code()) {
                ((Member<uint32_t>*)members.back().get())->range_from = from;
                ((Member<uint32_t>*)members.back().get())->range_to = to;
            }
            return *this; 
        
        }

        uint32_t size() override { 
            
            if (members.size() == 1) { return members[0]->size(); } 
            
            uint32_t size_v = 0;
            
            for (auto &m : members) size_v += m.get()->size();

            return size_v; 

        }

        std::string type() override { if (members.size() == 1) { return members[0]->type(); } return "Struct"; }

        bool owns(AnyMember& m) override { 

            for (auto &s : members) if (s.get() == &m) return true;

            return false;

        }

        void remove(const Struct& s) {

            for (auto it = members.begin(); it != members.end(); ++it) {

                if ((*it).get() == &s) {

                    size_v -= (*it)->footprint_all();
                    members.erase(it);

                    break;

                }
            }
        }
        
         void each(std::function<void(AnyMember& m, int offset)> cb, int offset = 0) override {

            for (int i = 0 ; i < quantity; i++) {

                AnyMember::each(cb, offset);

                int size = 0;

                if (members.size() > 1 || members[0].get()->name().length() ||  striding()) {
                
                    for (auto &m :members) {
                        
                        m.get()->each(cb, offset+size);

                        size+=m.get()->footprint_all();

                    }

                    if (i!=quantity-1) offset+=footprint();
                    
                }
                
            }

        }

        void print(int tab = 0) override {

            AnyMember::print(tab);

            int offset = 0;

            if (members.size() > 1 || members[0].get()->name().length() ||  striding()) {
            
                for (auto &m :members) {
                    
                    m.get()->print(tab+1);

                    offset+=m.get()->footprint_all();

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
        
        virtual Struct& addPtr(std::shared_ptr<AnyMember> s) {

            members.push_back(s);

            size_v += members.back()->footprint_all();

            update();

            return *this;

        } 
        virtual Struct& removePtr(std::shared_ptr<AnyMember> s) {
            
            // auto it = std::remove(members.begin(), members.end(), s);

            // if (it == members.end()) {
                
            //     PLOGW << "error " << s.get()->name(); 
                
            //     return *this;
            
            // }

            // members.erase(it, members.end());

            // size_v -= members.back()->footprint_all();

            // update();

            return *this;

        } 

    };




};