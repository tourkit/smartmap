/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


#include "engine.hpp"
#include "gui.hpp"
#include "imgui.h"


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

    // TestWin test(engine.gui_v);

    engine.open("project.json");  

    engine.run();

}
