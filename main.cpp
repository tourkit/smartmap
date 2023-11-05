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

    SmartMap sm;

    struct FPSWidget : GUI::Window {

       FPSWidget() :  GUI::Window("FPS") {


       }

       void draw() override {


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

    };// fpsw;
    struct DebugWidget : GUI::Window {

        SmartMap *sm;

       DebugWidget(SmartMap *sm) :  GUI::Window("debug") {

            this->sm = sm;

       }

       void draw() override {

            ImGui::SliderInt("GL_BLEND_MODE_IN2",&sm->GL_BLEND_MODE_IN2,0,sm->GL_BLEND_MODES.size());
            ImGui::SliderInt("GL_B2LEND_MODE_OUT2",&sm->GL_BLEND_MODE_OUT2,0,sm->GL_BLEND_MODES.size()); 
            ImGui::SliderInt("GL_BLEND_MODE_IN",&sm->GL_BLEND_MODE_IN,0,sm->GL_BLEND_MODES.size());
            ImGui::SliderInt("GL_B2LEND_MODE_OUT",&sm->GL_BLEND_MODE_OUT,0,sm->GL_BLEND_MODES.size()); 
            ImGui::SliderInt("GL_BLEND_MODE_IN3",&sm->GL_BLEND_MODE_IN3,0,sm->GL_BLEND_MODES.size());
            ImGui::SliderInt("GL_B2LEND_MODE_OUT3",&sm->GL_BLEND_MODE_OUT3,0,sm->GL_BLEND_MODES.size()); 
            
       }

    } dw(&sm);


    FrameBuffer ndi_fb(1920,1080);
    NDI::Sender ndisender(ndi_fb.width,ndi_fb.height);
    engine.stack.list.push_back(new Stack::Action([&](){

        std::vector<unsigned char> data;
        data.resize(1920*1080*4);

        glBindFramebuffer(GL_FRAMEBUFFER, Engine::getInstance().fb->id);

        glReadPixels(0,0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

        ndisender.send(&data[0],data.size());

    }));

    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();


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


