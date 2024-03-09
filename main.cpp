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


int main() {

using namespace TEST;

    logger.cout();


    Struct& Rect = Struct::create("Rect").add<vec2>("pos").add<vec2>("size");
    Struct& rectangle = Struct::create("rectangle").add<vec2>("pos").add<vec2>("size").add<float_>("angle");
    // Struct& ID = Struct::create("ID").add<ui>();
    
    Buffer buff;

    Struct quad("myquad",2);

    quad.add(rectangle);

    quad.remove(rectangle);

    quad.add(Rect);

    buff.add(quad);
    
    buff.add(rectangle);

    Rect.add<float_>("angle").range(0.0f,1.0f);

    quad.add<ui>("somadem").range(100,200);
    
    quad.striding(true);

    Rect.striding(true);

    quad.resize(3);
    
    buff.print();

    std::string tab;
    buff.each([&](AnyMember& m, int offset, int depth){ 
        
        tab = "";
        for (int i = 0 ; i < depth; i++) tab+= "    ";
        std::string str = tab; 
        str += m.type() + " " + m.name();
        str += "    " +  std::to_string(offset);
        str += " (" +std::to_string(m.footprint())+")";

        PLOGD << str;

    }, [&](AnyMember& m){ if (m.striding()) PLOGD << tab<<"stride" << m.stride(); });
    
    buff["myquad"].eq(0)["Rect"]["size"].set<uint32_t>(123);
    auto ptr = buff.data.data();
    std::string str;
    for (int i = 0 ; i < 48; i++) str += " "+std::to_string(*(uint8_t*)(ptr+i));
    PLOGD << buff["myquad"].eq(1)["Rect"]["size"].offset;
    PLOGD << buff["rectangle"]["size"].offset;
    PLOGD << "out" << str << buff["myquad"].eq(0)["Rect"]["size"].get<uint32_t>();

    // todo : hardcopy/bkp/remap, struct to layout
 
}


