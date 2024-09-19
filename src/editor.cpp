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

    // static TestWin test(gui,10);

    auto w = ImGui::GetWindowWidth(); 
    auto x = 0;ImGui::GetCursorPosX();
    std::string name = selected->nameSTL();
    auto tx = ImGui::CalcTextSize(name.c_str()).x;
    if (ImGui::GetWindowDockNode() && !ImGui::GetWindowDockNode()->IsHiddenTabBar() && ImGui::DockNodeBeginAmendTabBar(ImGui::GetWindowDockNode())) {

        auto x = ImGui::GetCursorPosX();
        auto y = ImGui::GetCursorPosY();
        ImGui::SetCursorPosX(x+w-60);
        ImGui::SetCursorPosY(y-21);  
        auto &io = ImGui::GetIO();
        ImGui::SetWindowFontScale(.65);
        ImGui::Checkbox(("##locked"+std::to_string((size_t)this)).c_str(), &locked);
        ImGui::SetWindowFontScale(1);
        ImGui::SetCursorPosX(x+w+1-tx-65);
        ImGui::SetCursorPosY(y-21);

        ImGui::PushStyleColor(ImGuiCol_Text, selected->color);
        ImGui::Text(name.c_str());

        if (ImGui::IsItemClicked()) 
        ImGui::OpenPopup("nodecolorpicker");
        if (ImGui::BeginPopup("nodecolorpicker")) { 
            ImGui::ColorPicker4("#nodecolorpickercolor", &selected->color.x); 
            ImGui::EndPopup(); 
        }

        ImGui::PopStyleColor(1);

        ImGui::SetCursorPosY(y);

        ImGui::DockNodeEndAmendTabBar();
      }
    
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
