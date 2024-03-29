#include "tree.hpp"
#include "node.hpp"
#include "engine.hpp"
#include "imgui_internal.h"

TreeWidget::TreeWidget(Node* selected) : GUI::Window("Tree"), selected(selected) {  }

void TreeWidget::draw()  { 

    
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(4,1));

    if (!selected) selected = engine.tree;

    std::string search_str = "filter";
    ImGui::PushItemWidth(-1);
    ImGui::InputText("###filtersearch", &search_str[0], search_str.size(), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();

    // Create the table
   
    if (ImGui::BeginTable("TreeTable", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        
        drawChildrens(selected); 

        ImGui::EndTable();
    }
    ImGui::PopStyleVar(1);
}

void TreeWidget::drawChildrens(Node* node) { 

    for (auto child : node->childrens) drawNode(child);

}

 

bool TreeWidget::TreeViewNode(Node* node) {
using namespace ImGui;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick ;

    if (!node->childrens.size()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (node->open) flags |= ImGuiTreeNodeFlags_DefaultOpen;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return 0;

    // Layout
    // const ImVec2 curr_pos = ImVec2(0,GetCursorPos().y);
    const ImVec2 curr_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);

    const char *badbad = "mmmmmmmmmmmmmmmmmmmmmmmmmm";
    const ImVec2 text_size = CalcTextSize(badbad,badbad+15);
    ImRect bb(curr_pos.x, curr_pos.y, curr_pos.x + text_size.x, curr_pos.y + text_size.y);
    ItemSize(text_size, 0.0f);
    if (!ItemAdd(bb, 0))
        return 0;

    auto hovered = IsItemHovered();

    ImVec4 node_color = *(ImVec4*)&node->color;
    
    if(engine.selected != node) {

        if (hovered) node_color = ImVec4(1, .4, 0, 1);

        else if (engine.selected) node_color.w = .65; 

    }

    ImGui::PushStyleColor(ImGuiCol_Text, node_color);

    SameLine();
    SetCursorPosX(GetCursorPosX()-text_size.x);
    bool x = false;
    
    if (is_renaming != node) {x = TreeNodeEx(node->name.c_str(), flags);}
    else {

        // std::vector<char> name;
        // name.resize(512);
        // memset(&name[0],0,512);
        // memcpy(&name[0], node->name.c_str(), node->name.size());
        
        ImGui::InputText("##jksdhfjksdfjk", &node->name[0], node->name.size());
        
    }

           if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
    {
        if (ImGui::BeginMenu("add"))
        {
            if (ImGui::MenuItem("New")) {}
            ImGui::EndMenu();
        }
        bool will_exit = false;
        if (!is_deleting) {
        
            ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
            if(ImGui::MenuItem("delete")){ is_deleting = true; }
            ImGui::PopItemFlag();
        
        }else {

            if(ImGui::MenuItem("Sure ?")){
                
                is_deleting = false;
                auto parent = node->parent();
                delete node;
                if (parent) parent->update();
                
            }

        }

        if (!ImGui::IsItemHovered()) is_deleting = false;

        if(ImGui::MenuItem("rename")) is_renaming = node;

        if(ImGui::MenuItem("zoom")) engine.gui->trees[0]->selected = node;
      
        if(ImGui::MenuItem("pop")) engine.gui->trees.push_back(new TreeWidget(node));

        ImGui::EndPopup();
    }
    
    PopStyleColor();

        if (ImGui::BeginDragDropSource()) {

            auto ptr = (uint64_t)node;
            ImGui::SetDragDropPayload("_TREENONODE", &(ptr), sizeof(uint64_t));
            ImGui::Text(node->name.c_str());
            ImGui::EndDragDropSource();
            
        }else {

            static bool mouse_down = false;

            static Node* s = nullptr;

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) { mouse_down = true; s = node; }

            if (mouse_down) if (ImGui::IsMouseReleased(0)) Engine::getInstance().selected = s;
            
            if (ImGui::IsMouseReleased(0)) mouse_down = false;

        }

        if (ImGui::BeginDragDropTarget()) {     

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE")) node->add((Node*)(*(uint64_t*)payload->Data));

            ImGui::EndDragDropTarget();
            
        }

    SameLine();
    
    SetCursorPosX(GetWindowWidth()-30); 
     
    std::string str = "##active"+std::to_string(node->uid);
    Checkbox(str.c_str(), &node->is_active);

    return x;

}


void TreeWidget::drawNode(Node* node) { 

    if (node->hidden) return;
    
    ImGui::TableNextRow();

    if (ImGui::TableNextColumn()) {

        ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

        const bool recurse = TreeViewNode(node);


    // if(!ImGui::IsPopupOpen("#popup")){is_deleting = false;}

        const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        
        if (recurse) {


            ImDrawList* drawList = ImGui::GetWindowDrawList();
            verticalLineStart.x+=7;
            verticalLineStart.y+=-7;
            ImVec2 verticalLineEnd = verticalLineStart;
            
            verticalLineEnd.y+=14;
            ImVec2 verticalLineEnd2 = verticalLineEnd;
            verticalLineEnd2.x+=10;//Engine::getInstance().blank[8];
            drawList->AddLine(verticalLineStart, verticalLineEnd, IM_COL32(122,122,122,122));
            drawList->AddLine(verticalLineEnd, verticalLineEnd2, IM_COL32(122,122,122,122));

            if (ImGui::IsDragDropActive()) {
                                
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,ImVec2(4,0));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(4,0));

                ImVec2 dropperline;
                dropperline.x = ImGui::GetWindowPos().x;
                dropperline.y = ImGui::GetCursorScreenPos().y;
                dropperline.y += 2;

                ImVec2 dropperline2 = dropperline;
                dropperline2.x += ImGui::GetWindowWidth();
                dropperline2.y -= 10;

                ImGui::SameLine();
                ImGui::PushID(6969);
                ImGui::BeginGroup();
                drawList->AddRectFilled(dropperline, dropperline2, IM_COL32(255,0,0,30));
                ImGui::EndGroup();
                ImGui::PopID();

                if (ImGui::BeginDragDropTarget()) {     

                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENONODE"))PLOGI << "TODO MOVE NODE";

                    ImGui::EndDragDropTarget();
                }

                ImGui::PopStyleVar(2);
                    
            }

            drawChildrens(node);

            ImGui::TreePop();

        }

        
        
    
    }

}