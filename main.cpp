/*

    SMARTMAP (beta)

                        */


#include "log.hpp"
#include <memory>
#include <unordered_set>
#include <set>
#include <typeindex>




#include "struct2.hpp"
#include "buffer2.hpp"
#include "instance2.hpp"


struct ui { uint32_t v; };
struct float_ { float v; };
struct vec2 { float x = 0, y = 0; };
struct vec3 { float x = 0, y = 0, z = 0; };
struct vec4 { float x = 0, y = 0, z = 0, w = 0; };
 //
 
namespace TEST {   
    

struct OBJ {

    std::string name;

    static inline std::unordered_set<std::shared_ptr<OBJ>> pool;

    OBJ(std::string name) : name(name) {}
    ~OBJ() { PLOGD << "~"<<name; }

    static OBJ& create(std::string name) {

        auto obj = std::make_shared<OBJ>(name);

        pool.insert(obj);

        return *obj.get();

    }

    static void destroy(std::shared_ptr<OBJ> to_destroy) {

        auto x = pool.erase(to_destroy);

        if (!x) PLOGW << to_destroy.get()->name << " not found";

    }

    std::vector<std::shared_ptr<OBJ>> childrens;

    OBJ& add(OBJ& to_add) {

        for (auto &c : pool) if (c.get() == &to_add) {
            
            childrens.push_back(std::shared_ptr<OBJ>(c));
            
            return *this;

        }

        PLOGW << " noadd" << to_add.name;

        return *this;

    }

    OBJ& remove(OBJ& to_remove){

        bool found = false;
        for (auto &c : childrens) if (c.get() == &to_remove) {
            
            childrens.erase(std::remove(childrens.begin(),childrens.end(),c),childrens.end());
            found = true;
            break;

        };
  
        if (!found) PLOGW << " normove" << to_remove.name;

        return *this;

    }
};


};

int main() {

    // todo : delete , auto create, 

using namespace TEST;

    logger.cout();


    Struct& Rect = Struct::create("Rect").add<vec2>("pos").add<vec2>("size");
    Struct& rectangle = Struct::create("rectangle").add<vec2>("pos").add<vec2>("size").add<float_>("angle");
    // Struct& ID = Struct::create("ID").add<ui>();
    
    Buffer buff;

    // buff.add(rectangle);
    // buff.add(Rect);

    Struct quad("myquad",2);

    quad.add(rectangle);
    // quad.remove(rectangle);
    quad.add(Rect);
    buff.add(quad);
    quad.striding(true);

    rectangle.striding(true);
    
    buff.print();

    buff.each([&](AnyMember& m, int offset){ 
        
        std::string str;
        str += m.type() + " " + m.name();
        str += " " +  std::to_string(offset);
        str += " (" +std::to_string(m.footprint())+")";
        PLOGD << str;

    });

    // set
    
    buff["myquad"].eq(0)["Rect"]["size"].set<uint32_t>(123);

    auto ptr = buff.data.data();
    
    std::string str;
    for (int i = 0 ; i < 48; i++) str += " "+std::to_string(*(uint8_t*)(ptr+i));

    PLOGD << buff["myquad"].eq(1)["Rect"]["size"].offset;

    PLOGD << "out" << str << buff["myquad"].eq(0)["Rect"]["size"].get<uint32_t>();
 
}


