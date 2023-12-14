#include "../gui.hpp"
#include "../fps.hpp"


    struct FPSWidget : GUI::Window {

       FPSWidget() :  GUI::Window("WIDGETS") {


       }

       void draw() override {

            int c = 0;
            for (auto w:GUI::Window::pool)  { ImGui::Checkbox((w->name+"##"+std::to_string(c++)).c_str(), &w->active); }

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

    };