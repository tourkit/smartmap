#include "editor.hpp"
#include "engine.hpp"


EditorWidget::EditorWidget() : GUI::Window("Editor")  {  }

void EditorWidget::draw()  { 
    
    if (!engine.selected) return;

    std::string name = engine.selected->name;

    if (engine.selected->parent()) { name = engine.selected->parent()->name + "::" + name;  }
    // auto last_parent = selected->parent();  while (last_parent) { name = last_parent->name + "::" + name; last_parent = last_parent->parent(); }

    ImGui::Text("%s", name.c_str());
    
    // if (ImGui::BeginMenuBar()) { ImGui::EndMenuBar(); }

    engine.selected->editor();

}

