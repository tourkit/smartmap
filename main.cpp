#include "imgui/imgui.h"
#include "src/window.hpp"
#include "src/gui.hpp" 
#include "src/gui/ShadertoyWindow.h" 
#include "src/gui/TreeviewWindow.h" 

 
auto* window = new GL::Window(false,800,400,1120); 

auto* gui = new GUI{window->window}; 

struct TreeNode { 

    const char * name;
    
    std::set<TreeNode> tree;

  };

 TreeNode* current = nullptr; 
 

struct TEST_GUI : GUI::Renderer {

    void draw() override { 

        // ImGui::Begin("TEST");
        // ImGui::Text("hello world");
        // ImGui::BeginChild("child", ImVec2(0,50));
        // ImGui::Text("youpi");
        // ImGui::EndChild();
        // ImGui::BeginChild("child2", ImVec2(0,50));
        // ImGui::Text("youpi2");
        // ImGui::EndChild();
        // ImGui::End();

     }

} test_gui;

int main() {

    // ShadertoyWindow shadertoy1("smartmap"); 
    TreeviewWindow treeview1;

    treeview1.addNode(new GroupNode{"Repository"});
    auto* renderers = treeview1.addNode(new GroupNode{"Renderers"});
    renderers->addNode(new ShadertoyWindow("smartmap"));
    auto* dc = treeview1.addNode(new GroupNode{"DrawCalls"});
    dc->addNode(new Node{"DrawCall1"});
    dc->addNode(new GroupNode{"DrawCall2"});
    dc->addNode(new GroupNode{"DrawCall3"});


    while(true) window->render([&]() {

        // ImGui::ShowDemoWindow();

        gui->render();

 
    });

} 

