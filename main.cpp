/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"

int main() { 


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

    // Texture img("boy.jpg");
    // FrameBuffer fb;
    // auto *shader = new ShaderProgram({"smartmap.frag", "smartmap.vert"});
    // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, shader, &img, &fb, "Quad to layer"});
    
    
    StackWidget sw(&engine.stack);

    FileWidget fw;

    Engine::Run();
 
}


