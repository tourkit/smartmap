/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "ndi.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"

// shader sources
// clocks
// fix feedback blending

int main() { 



    auto &engine = Engine::getInstance();
    engine.init();

    SmartMap::Base sm;

    struct FPSWidget : GUI::Window {

       FPSWidget() :  GUI::Window("FPS") {


       }

       void draw() override {

            int c = 0;
            for (auto w:GUI::Window::pool) { ImGui::Checkbox((w->name+"##"+std::to_string(c++)).c_str(), &w->active); }

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



    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();


    sm.config.import("config.json");


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


