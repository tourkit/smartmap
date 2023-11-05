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

    struct FPSWidget : GUI::Window {

        Buffer::Object::Entry  specs_ubo;
        int sequid = 1000;

       FPSWidget() :  GUI::Window("FPS") {

            specs_ubo = Engine::getInstance().dynamic_ubo.buffer.add("infos", {"int","int","int","int"},1)->create();
            
            // memcpy(x->data(), &sequid, 4);
            // sequid = std::modf(sequid,1000);

       }

       void draw() override {

            specs_ubo.set<uint64_t>(0,sequid--);
            if (!sequid) sequid = 1000;;

            ImGui::Text(std::to_string(ImGui::GetIO().Framerate).c_str());  
            // std::cout << ImGui::GetIO().Framerate << std::endl;
             for (auto fps:FPS::pool) {
                
                if (fps->fps > 1) {

                    ImGui::Text((fps->name+": "+std::to_string((GLuint)(fps->fps))+" FPS").c_str());
                    if (fps->has_dropped) {

                        ImGui::SameLine();
                        ImGui::Text(" - DROPPED !");
                    }
                
                }
                
            }

       }

    } fpsw;

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
    
    
    // StackWidget sw(&engine.stack);

    // FileWidget fw;

    Engine::Run();
 
}


