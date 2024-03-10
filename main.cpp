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

namespace TEST {

std::string glsl_layout(AnyMember& s) {

    auto bkp_srtiding = s.striding();

    s.striding(true);

    if (!s.members.size()) return "";

    std::string str = "struct " + s.name() + " {";

    for (auto &m : s.members) {
        
        std::string name = !m->typed() ? "struct" : m->type().name();
    
        str += " " + name + " " + m->name() + ";";
    
    }

    for (int i = 0; i < s.stride()/sizeof(float); i++) str += " float stride" + std::to_string(i) + ";";
    
    str += " };";
    
    s.striding(bkp_srtiding);

    return str;

}

};


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
    
    buff["myquad"].eq(0)["Rect"]["size"].set<uint32_t>(123);

    auto ptr = buff.data.data();
    std::string str;
    for (int i = 0 ; i < 48; i++) str += " "+std::to_string(*(uint8_t*)(ptr+i));
    PLOGD << "out" << str << buff["myquad"].eq(0)["Rect"]["size"].get<uint32_t>();

    PLOGD << buff["myquad"].eq(1)["Rect"]["size"].offset;
    PLOGD << buff["rectangle"]["size"].offset;

    for (auto &m : quad.members) {
        
        auto x = glsl_layout(*m);

        if (x.length()) PLOGD << x;
        
    }

    PLOGD << glsl_layout(quad);

    auto bkp = buff.copy();

    // bkp.print();

    buff["myquad"].eq(0)["Rect"]["size"].set<uint32_t>(245);
    PLOGD<<bkp["myquad"].eq(0)["Rect"]["size"].get<uint32_t>();
    PLOGD<<buff["myquad"].eq(0)["Rect"]["size"].get<uint32_t>();

 
}


