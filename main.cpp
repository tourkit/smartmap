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

    // buff.add(rectangle);
    // buff.add(Rect);


    Struct quad("myquad",2);
    quad.add(rectangle);
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
    

    PLOGD << buff["myquad"].eq(1)["Rect"]["size"].offset;

    PLOGD << "out" ;
 
}


