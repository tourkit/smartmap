/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;



int main() { 

    // Boilerplate();

    auto &engine = Engine::getInstance();
    engine.init();

    SmartMap sm;

    sm.import("config.json");

    std::cout <<  "\nSHADERS HEADER:" << std::endl;
    for (auto ubo:UBO::pool) { 

        std::string str = "layout (binding = ";
        str += std::to_string(ubo->binding);
        str += ", std140) uniform ";
        str += ubo->name;
        str += " { ";
        
        int i = 0; for (auto obj:ubo->buffer.objects) { 

            str += obj.name;
            str += " ";
            for(int i = 0; i < obj.name.length(); i++) str += std::tolower(obj.name[i]); 
            str += "[";
            str += std::to_string(obj.reserved);
            str += "]; ";

         }

        str += " };";
        std::cout <<  str << std::endl;

    }

    // ArtnetWidget aw(sm.artnet);


    // auto *shader = new ShaderProgram({"basic.frag", "smartmap.vert"});
    // auto *shader2 = new ShaderProgram({"red.frag", "smartmap.vert"});


    // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, shader2, nullptr, nullptr, "Quad to layer"});
    // engine.stack.list.push_back(new Stack::Action{[&](){

    //     Engine::getInstance().fb->clear();
    //     shader->use();
    //     Engine::getInstance().quad->draw();


    // }, "SM layers al at once :("});

    StackWidget sw(&engine.stack);

    FileWidget fw;

    Engine::Run();
 
}


