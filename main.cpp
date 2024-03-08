/*

    SMARTMAP (beta)

                        */


#include "log.hpp"
#include <memory>
#include <unordered_set>
#include <set>
#include <typeindex>



int nextFactor(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }


namespace TEST {

    struct AnyMember {

        static inline std::unordered_set<std::shared_ptr<AnyMember>> pool;

        AnyMember(std::string name_v = "") { name(name_v); pool.insert(std::shared_ptr<AnyMember>(this)); }
        ~AnyMember() { pool.erase(std::shared_ptr<AnyMember>(this)); }

        uint32_t quantity = 1; // ! \\ ONLY FOR Array !

        std::shared_ptr<AnyMember> owner = nullptr;

        virtual bool owns(AnyMember& m) { return false; }

        virtual void update() { 
            
            if (owner) {

                auto topmost = owner.get(); 

                topmost->update();
                
                while ( topmost->owner ) { topmost = topmost->owner.get(); }  

            }

        }

        virtual uint32_t size() { return 0; }

        virtual uint32_t footprint() { 

            if (is_striding) return nextFactor(size_v,16);
            
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
    struct Instance : AnyMember {

        Struct* s;

        std::string name() override { if (quantity > 1) { return AnyMember::name()+"_"+std::to_string(quantity); } return AnyMember::name(); }

    };

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

        Struct copy() {

            Struct s = * this;
            // for (auto &m : members) // make copry

            return s;

        }

        std::vector<std::shared_ptr<AnyMember>> members;

        Struct& add(Struct& s) { return addPtr(std::shared_ptr<Struct>(&s)); }  
    
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

        uint32_t size() override { if (members.size() == 1) { return members[0]->size(); } return AnyMember::size(); }

        std::string type() override { if (members.size() == 1) { return members[0]->type(); } return "Struct"; }

        // bool owns(AnyMember& m) override { 

        //     for (auto &s : members) if (s.get() == &m) return true;

        //     return false;

        // }

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

                int stride = nextFactor(offset,16)-offset;
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

            s.get()->owner = std::shared_ptr<Struct>(this);

            size_v += members.back()->footprint_all();

            update();

            return *this;

        } 

    };

    struct Buffer : Struct{

        std::vector<char> data;
        
        Buffer() : Struct("buffer_definition") {

            data.reserve(1000);
            memset(data.data(),0,data.size());

        }

        void set(AnyMember& m, void* data) { }

        void update() override { data.resize( footprint_all() ); Struct::update(); }

    };

};

struct ui { uint32_t v; };
struct float_ { float v; };
struct vec2 { float x = 0, y = 0; };
struct vec3 { float x = 0, y = 0, z = 0; };
struct vec4 { float x = 0, y = 0, z = 0, w = 0; };

int main() {

using namespace TEST;

    logger.cout();

    Struct& Rect = Struct::create("Rect").add<vec2>("pos").add<vec2>("size");
    Struct& rectangle = Struct::create("rectangle").add<vec2>("pos").add<vec2>("size").add<float_>("angle");
    Struct& ID = Struct::create("ID").add<ui>();

    Struct quad("myquad",2);

    quad.add(Rect);
    Rect.add<float_>("angle");

    quad.striding(true);

    Rect.striding(true);

    Buffer buff;

    buff.add(quad);

    buff.print();

    AnyMember* last_owner = nullptr;

    int depth = 0;

    buff.each([&](AnyMember& m, int offset){ 

        if (m.owner.get() != last_owner) {
            
            if (m.owner.get()->owner.get() == last_owner) { depth++; }

            else {

                while(m.owner.get() != last_owner) {

                    depth--;

                    last_owner = last_owner->owner.get(); 

                }
                
            }
            
            
        }
        
        std::string str;
        for (int i = 0; i < depth; i++) str += "  ";
        str += m.type() + " " + m.name();
        str += " " + std::to_string(m.footprint());
        str += " " + std::to_string(offset);
        PLOGD << str;

        // auto owner = m.owner.get();
        // if (owner && !m.size()) {

        //     auto s = (Struct*)owner;

        //     if (s->members.size() && &m == s->members.back().get()) {
                
        //         std::string str;
        //         for (int i = 0; i < depth; i++) str += "  ";
        //         str+=m.name()+ " "+s->name()+ " "+s->members.back().get()->name()+ " stride";
        //         // PLOGD << str;
                
        //     }

        // } 

        last_owner = m.owner.get();

    });

    // set
    
    PLOGD << "out";
 
}


