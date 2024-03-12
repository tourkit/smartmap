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
    
    Buffer buff;

    Struct quad("myquad");

    quad.add(Rect);

    buff.add(quad);

    PLOGD << "BKP";

    auto bkp = buff.copy();

    /// is myquad copyed ? compre adresses

    PLOGD << (buff["myquad"].member == bkp["myquad"].member ? "ARGH" : "OUF");

    PLOGD <<"out";

    return 0;
 
}
