// add buttons/menu to title bar

#include "imgui/imgui.h"
#include "src/window.hpp"
#include "src/gui.hpp" 
#include "src/gui/ShadertoyWindow.h" 
#include "src/gui/TreeviewWindow.h" 
#include "src/renderer.hpp" 
 
auto* window = new GL::Window(false,800,400,1120); 

auto* gui = new GUI{window->window}; 

struct TEST_GUI : GUIRenderer {
    
    TEST_GUI() {

    }

    void draw() override { 

        ImGui::ShowDemoWindow();

     }

} test_gui;

int main() {

    new Shadertoy("smartmap"); 

    TreeviewWindow tree;
    tree.addPool<DrawCall>("DrawCalls");
    tree.addPool<Renderer>("Renderers");
    tree.addPool<Repository>("Repository");

    while(true) window->render([&]() { gui->render(); });

} 

