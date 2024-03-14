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

using namespace TEST;

    logger.cout();

    Struct& rectangle = Struct::create("rectangle").add<vec2>("reca").add<vec2>("recb");

    Buffer buff;

    Struct wrapper("wrapper",2);
    
    buff.add(wrapper);

    wrapper.add(rectangle);

    PLOGD << buff.data.size();

    buff["wrapper"].eq(1)["rectangle"]["reca"].set<uint32_t>(123);
    
    auto &bkp = buff.copy();

    bkp["wrapper"].eq(0)["rectangle"]["reca"].set<uint32_t>(123);

    buff.remap(bkp); 

    std::string str;

    for (auto i = 0; i<buff.data.size(); i++) str += std::to_string((uint8_t)buff.data[i]) + " ";

    PLOGD << str;

    return 0;

 
}