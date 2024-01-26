#pragma once
#include "gui.hpp"
#include "node.hpp"

struct EditorWidget : GUI::Window {

    Node* selected = nullptr;

    EditorWidget() : GUI::Window("Editor")  {  }

    void draw() override { 
        
        if (!selected) return;

        std::string name = selected->name;
        if (selected->parent()) { name = selected->parent()->name + "::" + name;  }
        // auto last_parent = selected->parent();  while (last_parent) { name = last_parent->name + "::" + name; last_parent = last_parent->parent(); }

        ImGui::Text("%s", name.c_str());
        
        // if (ImGui::BeginMenuBar()) { ImGui::EndMenuBar(); }

        selected->editor();
    
    }

};