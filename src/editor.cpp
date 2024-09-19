#include "editor.hpp"

#include "imgui.h"
#include "node.hpp"
#include "gui.hpp"

#include "vendors/imgui/imgui_internal.h"

#include <ctime>
#include <cstring>




/////// EditorWidget.hpp

EditorWidget::EditorWidget(GUI* gui) : GUI::Window("Editor", gui)  {  }

EditorWidget::~EditorWidget()  { std::erase_if(gui->editors, [this](EditorWidget* e) { return e == this; }); }


void EditorWidget::draw() {

    if (!selected || !locked) selected = gui->selected;
    if (!selected) return;


    // static TestWin testwin("test", gui);

    // testwin.floats.resize(4);

    // ImGuiContext& g = *GImGui;
    // auto curr_node =ImGui::GetCurrentWindow()->DockNode;
    // ImGui::GetCurrentWindow()->...
    // if (ImGui::GetCurrentWindow()->DockTabIsVisible && ImGui::DockNodeBeginAmendTabBar(curr_node)) {
    // ImGui::SameLine();

    std::string name = selected->nameSTL();

    ImGui::PushStyleColor(ImGuiCol_Text, selected->color);
    ImGui::Text(this->name.c_str());

    if (ImGui::IsItemClicked()) ImGui::OpenPopup("nodecolorpicker");
    if (ImGui::BeginPopup("nodecolorpicker")) { ImGui::ColorPicker4("#nodecolorpickercolor", &selected->color.x); ImGui::EndPopup(); }

    ImGui::SameLine(); ImGui::Checkbox(("lock##locked"+std::to_string((size_t)this)).c_str(), &locked);

    ImGui::PopStyleColor(1);

    // ImGui::DockNodeEndAmendTabBar();}
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    std::string referings;
    for (auto r : selected->referings) if (r) referings += (r->type_name()+"::"+r->name())+", ";
    if (referings.length()) {
        std::string str = "(refering"+std::string(referings.length()>1?"s":"")+": "+referings.substr(0,referings.length()-2)+")";
        ImGui::SameLine(); ImGui::Text(str.c_str()); }
    ImGui::PopStyleColor(1);


    if(EDITOR::ptr.find(selected->stored_type) != EDITOR::ptr.end()) 

        (*(std::function<void(Node*,void*)>*)EDITOR::ptr[selected->stored_type])(selected,selected->void_ptr);


}
