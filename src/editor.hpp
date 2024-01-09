#pragma once
#include "gui.hpp"
#include "node.hpp"

struct EditorWidget : GUI::Window {

    EditorWidget() : GUI::Window("Editor")  {  }

    void draw() override { 
        
        // if (ImGui::BeginMenuBar()) { ImGui::EndMenuBar(); }
    
    }

};