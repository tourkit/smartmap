#include "editor.hpp"
#include "engine.hpp"


EditorWidget::EditorWidget() : GUI::Window("Editor")  {  }

void EditorWidget::draw()  { 
    
    if (!selected || !locked) selected = engine.selected;

    std::string name = selected->name;

    if (selected->parent()) { name = selected->parent()->name + "::" + name;  }
    // auto last_parent = selected->parent();  while (last_parent) { name = last_parent->name + "::" + name; last_parent = last_parent->parent(); }

    ImGui::Text("%s", name.c_str()); ImGui::SameLine(); ImGui::Checkbox("lock##locked", &locked);
    
    // if (ImGui::BeginMenuBar()) { ImGui::EndMenuBar(); }

    selected->editor();

}

