#include "editor.hpp"

#include "imgui.h"
#include "node.hpp"
#include "gui.hpp"
#include "engine.hpp"

#include <ctime>
#include <cstring>




/////// EditorWidget.hpp

EditorWidget::EditorWidget() : GUI::Window("Editor")  {  }

EditorWidget::~EditorWidget()  { std::erase_if(engine.gui->editors, [this](EditorWidget* e) { return e == this; }); }


void EditorWidget::draw() {

    if (!selected || !locked) selected = Node::selected;

    std::string name = selected->name();
    // this->name = name;
    auto parent = selected->parent();
    if (parent) {
        name = parent->name() + "::" + name;  }

    ImGui::PushStyleColor(ImGuiCol_Text, selected->color);
    ImGui::Text("%s", name.c_str());
    ImGui::PopStyleColor(1);

    if (ImGui::IsItemClicked()) ImGui::OpenPopup("nodecolorpicker");
    if (ImGui::BeginPopup("nodecolorpicker")) { ImGui::ColorPicker4("#nodecolorpickercolor", &selected->color.x); ImGui::EndPopup(); }

    ImGui::SameLine(); ImGui::Checkbox(("lock##locked"+std::to_string((size_t)this)).c_str(), &locked);


    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    std::string referings;
    for (auto r : selected->referings) if (r) referings += (r->name())+", ";
    if (referings.length()) {
        std::string str = "(refering"+std::string(referings.length()>1?"s":"")+": "+referings.substr(0,referings.length()-2)+")";
        ImGui::SameLine(); ImGui::Text(str.c_str()); }
    ImGui::PopStyleColor(1);

    selected->editor();

}
