#pragma once
#include "gui.hpp"
#include "node.hpp"

struct EditorWidget : GUI::Window {


    Node* selected = nullptr;

    EditorWidget() : GUI::Window("Editor")  {  }

    void draw() override { 
        
        if (!selected) return;

        ImGui::Text("%s", selected->name.c_str());
        // if (ImGui::BeginMenuBar()) { ImGui::EndMenuBar(); }
    
    }

};