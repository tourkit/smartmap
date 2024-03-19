/*

    SMARTMAP (beta)

                        */

#include "log.hpp"
#include <memory>
#include <unordered_set>
#include <set>
#include <typeindex>

#include "member.hpp"
#include "struct.hpp"
#include "buffer.hpp"
#include "instance.hpp"

struct ui { uint32_t v; };
struct ui8 { uint8_t v; };
struct float_ { uint8_t v; };
struct vec2 { uint8_t x = 0, y = 0; };
struct vec3 { uint8_t x = 0, y = 0, z = 0; };
struct vec4 { uint8_t x = 0, y = 0, z = 0, w = 0; };

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

int main() {

    logger.cout();

    Struct &vertice = Struct::create("Vertice").add<vec2>("Position").add<vec2>("UV").add<ui8>("ID");
    
    Struct &indice = Struct::create("Indice").add<ui8>("Vertice1").add<ui8>( "Vertice2").add<ui8>("Vertice3");

    Struct vertices("Vertices",0);
    vertices.add(vertice);
    vertices.striding(true);

    Buffer vbo;
    vbo.add(vertices);

    // vbo[0].push
    // vbo["Vertices"].push()

    vbo.print();
    vbo.printData();








    
    
}