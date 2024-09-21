/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */



#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui/imgui_internal.h"

#include "engine.hpp"
#include "gui.hpp"
#include "editor.hpp"





struct slidetest : GUI::Window {

    using GUI::Window::Window;

    void draw() override {

        static float x=0;
        ImGui::SliderFloat("x", &x,0,1);

    }

};


int main() {

    // logger.cout(Sev::warning);

    engine.init();
    engine.open("project.json");  


    // TestWin test(engine.gui_v);


    engine.run();

}
