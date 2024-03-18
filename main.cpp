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

std::string glsl_layout(Member& s) {

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

    logger.cout();

using namespace TEST;

    Struct& rectangle = Struct::create("rectangle").add<vec2>("size").add<vec2>("pos");

    Struct& argb = Struct::create("argb").add<float_>("alpha").add<float_>("red").add<float_>("green").add<float_>("blue");

    Buffer buff;

    Struct quad("MyQuad", 2);

    rectangle.striding(true);
    
    quad.add(argb);

    buff.add(quad);
    
    quad.add(rectangle);

    rectangle.add<float_>("angle");

    buff.print();

    buff["MyQuad"].eq(0)["rectangle"]["size"].set<uint32_t>(123);

    auto &bkp = buff.copy();

    Struct inv("MyQuad", 2);
    inv.add(rectangle).add(argb);
    
    Buffer buff2;

    buff2.add(inv);
    
    // buff2.print();

    buff2.remap(buff); 

    buff["MyQuad"].eq(0)["rectangle"]["size"].set<uint32_t>(245);

    ((Struct*)bkp["MyQuad"]["rectangle"].member)->remove(bkp["MyQuad"]["rectangle"]["angle"].member);

    bkp.remap(buff); 


    buff["MyQuad"].eq(0)["rectangle"]["size"].set<uint32_t>(69);
    bkp["MyQuad"].eq(1)["rectangle"]["size"].set<uint32_t>(33);

    std::string str;

    str = "buff: " + std::to_string(buff.data.size()) + " : ";

    for (auto i = 0; i<buff.data.size(); i++) str += std::to_string((uint8_t)buff.data[i]) + " ";

    PLOGD << str;

    str = "buff2: " + std::to_string(buff2.data.size()) + " : ";

    for (auto i = 0; i<buff2.data.size(); i++) str += std::to_string((uint8_t)buff2.data[i]) + " ";

    PLOGD << str;

    str = "bkp: " + std::to_string(bkp.data.size()) + " : ";

    for (auto i = 0; i<bkp.data.size(); i++) str += std::to_string((uint8_t)bkp.data[i]) + " ";

    PLOGD << str;

    
}