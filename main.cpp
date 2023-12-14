/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "ndi.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
#include "widgets/fps.hpp"

// shader sources
// clocks
// fix feedback blending

int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();
    engine.init();

    SmartMap::Base sm;

    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();

    // sm.config.import("config.json");

    // std::cout <<  "\nSHADERS HEADER:" << std::endl;
    // for (auto ubo:UBO::pool) { 

    //     std::string str = "layout (binding = ";
    //     str += std::to_string(ubo->binding);
    //     str += ", std140) uniform ";
    //     str += ubo->name;
    //     str += " { ";
        
    //     int i = 0; for (auto obj:ubo->buffer.objects) { 

    //         str += obj.name;
    //         str += " ";
    //         for(int i = 0; i < obj.name.length(); i++) str += std::tolower(obj.name[i]); 
    //         str += "[";
    //         str += std::to_string(obj.reserved);
    //         str += "]; ";

    //      }

    //     str += " };";
    //     std::cout <<  str << std::endl;

    // }

    StackWidget sw(&engine.stack);

    FileWidget fw;

    FPSWidget fpsw;

    Engine::Run();
 
}


